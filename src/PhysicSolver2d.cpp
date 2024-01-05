#include "PhysicSolver2d.h"
#include <chrono>
#include <SFML/OpenGL.hpp>
#include <gl/glu.h>

#include <omp.h>

////ChunkGrid:

ChunkGrid::ChunkGrid(int cS, int wW, int wH) :
    cellSize{ cS }, grid_width{ wW / cS }, grid_height{ wH / cS }, window_width{wW}, window_height{ wH }
{
    // int gW = wW / cS;
    // int gH = wH / cS;
    grid = std::vector<Chunk>(grid_width * grid_height);
}

void ChunkGrid::assignGrid(const std::vector<PhysicBody2d*>& obj) {
    for (auto& i : grid) {
        i.clear();
    }

    for (auto& i : obj) {
        if(i->getPos().x >= 0 && i->getPos().x < window_width && i->getPos().y >= 0 && i->getPos().y < window_height){
            int x = int(i->getPos().x / cellSize);
            int y = int(i->getPos().y / cellSize);
            if (0 <= x && x < grid_width && 0 <= y && y < grid_height)
                grid.at(x + y * grid_width).push_back(i);
        }
    }
}

void ChunkGrid::updateChunkSize(const PhysicBody2d* obj) {
    if (obj->getRadius() * 2 > cellSize) {
        cellSize = (int)ceil(obj->getRadius() * 2);
        grid_width = window_width / cellSize;
        grid_height = window_height / cellSize;
        grid = std::vector<Chunk>(grid_width*grid_height);
    }
}

void ChunkGrid::update_collision() {
    for (int x{ 1 }; x < grid_width - 1; x++)
        for (int y{ 1 }; y < grid_height - 1; y++) {
            Chunk& cell = grid.at(x + y * grid_width);
            if (cell.size != 0)
                for (int i{ -1 }; i < 2; i++)
                    for (int j{ -1 }; j < 2; j++) {
                        auto& neigh_cell = grid.at(x + i + (y+j) * grid_width);
                        if (neigh_cell.size != 0)
                            solve_collision(cell, neigh_cell);
                    }
        }
}

void ChunkGrid::update_collision_mt() {
    #pragma omp parallel for num_threads(omp_get_num_procs()/2)
    for (int x{ 1 }; x < grid_width - 1; x++)
        for (int y{ 1 }; y < grid_height - 1; y++) {
            Chunk& cell = grid.at(x + y * grid_width);
            if (cell.isActive)
                for (int i{ -1 }; i < 2; i++)
                    for (int j{ -1 }; j < 2; j++) {
                        auto& neigh_cell = grid.at(x + i + (y+j) * grid_width);
                        if (neigh_cell.size != 0)
                            solve_collision(cell, neigh_cell);
                    }
        }
}

void ChunkGrid::solve_collision(Chunk& central_chunk, Chunk& neighboring_chunk) {
    for (auto& i : central_chunk) {
        for (auto& j : neighboring_chunk)
        {
            if (&i != &j) {
                if (collision_type == DEFAULT) {
                    Vec2 diff = i->getPos() - j->getPos();
                    float diffLen = diff.length();
                    float dist = diffLen - (i->getRadius() + j->getRadius());
                    Vec2 transform = diff / diffLen * (dist / 2) * 0.8f;
                    if (dist < 0) {
                        if (i->isKinematic) i->current_position -= transform;
                        if (j->isKinematic) j->current_position += transform;
                    }
                }
                else if (collision_type == FUNC) {
                    collision_function(i, j);
                }
                else if (collision_type == LAMBDA) {
                    collision_lambda(i, j);
                }
            }
        }
    }
}

int ChunkGrid::count() {
    int sum{};

    for (auto& i : grid) {
        sum += i.size;
    }
    return sum;
}

////PhysicSolver2d:

PhysicSolver2d& PhysicSolver2d::add(PhysicBody2d* obj) { //depraicated
    objects.push_back(obj);
    grid.updateChunkSize(obj);
    return *this;
}

PhysicSolver2d& PhysicSolver2d::add(Vec2 position, float size, bool isKinematic, sf::Color color) {
    int x = int(position.x / grid.getChunkSize());
    int y = int(position.y / grid.getChunkSize());
    if(5 <= x && x < grid.getWidth()-5 && 5 <= y && y < grid.getHeight()-5 && grid.getChunk(x, y).canAdd())//!grid.getChunk(x, y).isFull())
    {
        PhysicBody2d* obj = new PhysicBody2d(position, size, isKinematic, color);
        objects.push_back(obj);
        grid.updateChunkSize(obj);
        grid.assignGrid(objects);
    }
    return *this;
}
PhysicSolver2d& PhysicSolver2d::addLink(PhysicBody2d* obj1, PhysicBody2d* obj2, float len){
    PhysicLink2d* obj = new PhysicLink2d(obj1, obj2, len);
    links.push_back(obj);
    return *this;
}

void PhysicSolver2d::update(const float d_time, const int sub_step) {
    float sub_dt = d_time / sub_step;

    for (int i = 0; i < sub_step; i++) {
        std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
        update_acceleration();
        std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
        simResult[0] = std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count();
        begin = std::chrono::steady_clock::now();
        update_constraints();
        end = std::chrono::steady_clock::now();
        simResult[1] = std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count();
        begin = std::chrono::steady_clock::now();
        update_links();
        end = std::chrono::steady_clock::now();
        simResult[2] = std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count();
        begin = std::chrono::steady_clock::now();
        grid.assignGrid(objects);
        end = std::chrono::steady_clock::now();
        simResult[3] = std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count();
        begin = std::chrono::steady_clock::now();
        // grid.update_collision();
        grid.update_collision_mt();//multi_thread
        // update_collision();
        end = std::chrono::steady_clock::now();
        simResult[4] = std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count();
        begin = std::chrono::steady_clock::now();
        update_position(sub_dt);
        end = std::chrono::steady_clock::now();
        simResult[5] = std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count();
    }
}

void PhysicSolver2d::update_position(const float d_time)
{
    for (auto& i : objects) {
        if (i->getPos().x != i->getPos().x || i->getPos().y != i->getPos().y) {
            deleteObject(i);
        }
        else i->update_position(d_time);
    }
}

void PhysicSolver2d::update_acceleration() {
    if(acceleration_type == VALUE)
        for (auto& i : objects)
            i->accelerate(accelerationValue);
    else if(acceleration_type == FUNC)
        for (auto& i : objects)
            i->accelerate(acceleration_function(i, objects));
}

void PhysicSolver2d::update_constraints() {
    if (constraint_type == DEFAULT) {
        Vec2 sphere_centre(350, 350);
        float radius = 300;
        for (auto& i : objects) {
            if (i->isKinematic) {
                Vec2 diff = i->getPos() - sphere_centre;
                float diffLen = diff.length();
                if (diffLen + i->radius > radius) {
                    i->current_position -= diff / diffLen * (diffLen + i->radius - radius);
                }
            }
        }
    }
    else if (constraint_type == FUNC) {
        for (auto& i : objects)
            if(i->isKinematic)
                i->current_position = constraint_fun(i);
    }
}

void PhysicSolver2d::update_collision() {
    for (auto& i : objects) {
        for (auto& j : objects)
        {
            //if (&i == &j) break;
            if (&i != &j) {
                Vec2 diff = i->getPos() - j->getPos();
                float diffLen = diff.length();
                float dist = diffLen - (i->getRadius() + j->getRadius());
                if (dist < 0) {
                    i->current_position -= diff / diffLen * (dist / 2);
                    j->current_position += diff / diffLen * (dist / 2);
                }
            }
        }
    }
}

void PhysicSolver2d::set_acceleration(const Vec2 accVal) {
    acceleration_type = VALUE;
    accelerationValue = accVal;
}
void PhysicSolver2d::set_acceleration(std::function<Vec2(PhysicBody2d*, std::vector<PhysicBody2d*>&)> accFun) {
    acceleration_type = FUNC;
    acceleration_function = accFun;
}
void PhysicSolver2d::set_acceleration() {
    acceleration_type = NONE;
}

void PhysicSolver2d::set_constraints_def() {
    constraint_type = DEFAULT;
}
void PhysicSolver2d::set_constraints(std::function<Vec2(PhysicBody2d*)> conFun) {
    constraint_type = FUNC;
    constraint_fun = conFun;
}
void PhysicSolver2d::set_constraints() {
    constraint_type = NONE;
}

std::pair<bool, PhysicBody2d*> PhysicSolver2d::get_from_position(const Vec2& cord) {
    auto found = std::find_if(objects.begin(), objects.end(), [&cord](auto& i) {return i->isHere(cord); });
    if (found != objects.end()) {
        PhysicBody2d *r = *found;
        //objects.erase(found);
        return { true, r };
    }
    return { false, &PhysicBody2d::nullPB };
}

std::pair<bool, PhysicBody2d*> PhysicSolver2d::pop_from_position(const Vec2& cord) {
    auto found = std::find_if(objects.begin(), objects.end(), [&cord](auto& i) {return i->isHere(cord); });
    if (found != objects.end()) {
        PhysicBody2d *r = *found;
        objects.erase(found);
        return { true, r };
    }
    return { false, &PhysicBody2d::nullPB };
}

void PhysicSolver2d::deleteObject(PhysicBody2d* obj) {
    auto it = std::find(objects.begin(), objects.end(), obj);
    if (it != objects.end()) {
        objects.erase(it);
        // auto it2 = std::find_if(links.begin(), links.end(), [obj](auto& i) {return &i->getPB1() == obj || &i->getPB2() == obj; });
        // if (it2 != links.end()){
        //     delete(*it2);
        //     links.erase(it2);
        // }
        // multiple links
        for (auto it2 = links.begin(); it2 != links.end(); ) {
            if ((*it2)->getPB1() == obj || (*it2)->getPB2() == obj) {
                delete(*it2);
                it2 = links.erase(it2);
            }
            else
                ++it2;
        }
        delete(obj);
    }
}

void PhysicSolver2d::clear() {
    for (auto& i : objects) { delete(i); }
    for (auto& i : links) { delete(i); }
    objects.clear();
    links.clear();
}

long long PhysicSolver2d::getSimResult(const size_t i) {
    if(i<0 || i>5)
        return 0;
    return simResult[i];
}

//PhysicDrawer:

PhysicDrawer::PhysicDrawer(const PhysicSolver2d& pS, const Vec2 wS, const float cS) : physicSolver{ pS }, windowSize{ wS }, particleSize{cS}
{
    glPointSize(particleSize + 1.f + 2.f);
}

void PhysicDrawer::draw(sf::RenderTarget& target, sf::RenderStates states) const {
    // for (const auto& i : physicSolver.getChunkGrid().getGrid())
    // {
    //     for (const auto& j : i)
    //     {
    //         target.draw(j->getFigure(), states);
    //     }
    // }
    glBegin(GL_POINTS);
    for (const auto& i : physicSolver.objects)
    {
        glColor3f(i->getColor().r / 255.f, i->getColor().g / 255.f, i->getColor().b / 255.f);
        glVertex3f(i->getPos().x, windowSize.y - i->getPos().y, 0);
    }
    glEnd();

    glLineWidth(1.f);
    glBegin(GL_LINES);
    for (const auto& i : physicSolver.links)
    {
        //target.draw(i->getFigure(), states);
        glColor3f(i->getPB1()->getColor().r / 255.f, i->getPB1()->getColor().g / 255.f, i->getPB1()->getColor().b / 255.f);
        glVertex3f(i->getPB1()->getPos().x, windowSize.y - i->getPB1()->getPos().y, 0);
        glColor3f(i->getPB2()->getColor().r / 255.f, i->getPB2()->getColor().g / 255.f, i->getPB2()->getColor().b / 255.f);
        glVertex3f(i->getPB2()->getPos().x, windowSize.y - i->getPB2()->getPos().y, 0);
    }
    glEnd();
}