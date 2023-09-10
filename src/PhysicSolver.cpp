#include "PhysicSolver.h"

#include <iostream>

////ChunkGrid:

ChunkGrid::ChunkGrid(int cS, int wW, int wH) :
    cellSize{ cS }, grid_width{ wW / cS }, grid_height{ wH / cS }, window_width{wW}, window_height{ wH }
{
    int gW = wW / cS;
    int gH = wH / cS;
    grid = std::vector<std::vector<std::vector<PhysicBody2d*>>>(gW,
        std::vector<std::vector<PhysicBody2d*>>
        (gH)
        );
}

void ChunkGrid::assignGrid(std::vector<PhysicBody2d*>& obj) {
    for (auto& i : grid) {
        for (auto& j : i) {
            if (j.size() > 0) j.clear();
        }
    }

    for (auto& i : obj) {
        int x = i->getPos().x / cellSize;
        int y = i->getPos().y / cellSize;
        if (0 <= x && x < grid_width && 0 <= y && y < grid_height)
            grid.at(i->getPos().x / cellSize).at(i->getPos().y / cellSize).push_back(i);
    }
}

void ChunkGrid::updateChunkSize(PhysicBody2d* obj) {
    if (obj->getRadius() * 2 > cellSize) {
        cellSize = obj->getRadius() * 2 ;
        grid_width = window_width / cellSize;
        grid_height = window_height / cellSize;
        grid = std::vector<std::vector<std::vector<PhysicBody2d*>>>(grid_width,
            std::vector<std::vector<PhysicBody2d*>>
            (grid_height)
            );
    }
}

void ChunkGrid::update_collision() {
    for (int x{ 1 }; x < grid_width - 1; x++)
        for (int y{ 1 }; y < grid_height - 1; y++) {
            std::vector<PhysicBody2d*>& cell = grid.at(x).at(y);
            if (cell.size() != 0)
                for (int i{ -1 }; i < 2; i++)
                    for (int j{ -1 }; j < 2; j++) {
                        auto& neigh_cell = grid.at(x + i).at(y + j);
                        if (neigh_cell.size() != 0)
                            solve_collision(cell, neigh_cell);
                    }
        }
}

void ChunkGrid::update_collision_mt() {
    int split_num = 8;
    #pragma omp parallel for
    for (int t = 0; t < split_num; t++) {

        int b{ 1 + t * grid_width / split_num };
        int e{ ((t * 2 + 1) * grid_width - 1) / split_num / 2 + 1 };

        for (int x{ 1 + t*grid_width/split_num}; x < ((t * 2 + 1)* grid_width - 1) / split_num/2 + 1; x++) {
            if (x == grid_width - 1) break;
            for (int y{ 1 }; y < grid_height - 1; y++) {
                std::vector<PhysicBody2d*>& cell = grid.at(x).at(y);
                if (cell.size() != 0)
                    for (int i{ -1 }; i < 2; i++)
                        for (int j{ -1 }; j < 2; j++) {
                            auto& neigh_cell = grid.at(x + i).at(y + j);
                            if (neigh_cell.size() != 0)
                                solve_collision(cell, neigh_cell);
                        }
            }
        }
    }
    #pragma omp parallel for
    for (int t = 0; t < split_num; t++) {

        int b{ ((t * 2 + 1) * grid_width - 1) / split_num / 2 + 1 };
        int e{ ((t + 1) * grid_width - 1) / split_num + 1 };

        for (int x{ ((t * 2 + 1) * grid_width - 1) / split_num / 2 + 1 }; x < ((t + 1) * grid_width - 1) / split_num + 1; x++) {
            if (x == grid_width - 1) break;
            for (int y{ 1 }; y < grid_height - 1; y++) {
                std::vector<PhysicBody2d*>& cell = grid.at(x).at(y);
                if (cell.size() != 0)
                    for (int i{ -1 }; i < 2; i++)
                        for (int j{ -1 }; j < 2; j++) {
                            auto& neigh_cell = grid.at(x + i).at(y + j);
                            if (neigh_cell.size() != 0)
                                solve_collision(cell, neigh_cell);
                        }
            }
        }
    }
}

void ChunkGrid::solve_collision(std::vector<PhysicBody2d*>& central, std::vector<PhysicBody2d*>& neigh) {
    for (auto& i : central) {
        for (auto& j : neigh)
        {
            if (&i != &j) {
                if (collision_type == DEFAULT) {
                    Vec2 diff = i->getPos() - j->getPos();
                    float diffLen = diff.length();
                    float dist = diffLen - (i->getRadius() + j->getRadius());
                    if (dist < 0) {
                        if (i->isKinematic) i->current_position -= diff / diffLen * (dist / 2); // * 0.5; //squishiness
                        if (j->isKinematic) j->current_position += diff / diffLen * (dist / 2); // * 0.5;
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
        for (auto& j : i) {
            sum += j.size();
        }
    }
    return sum;
}

////PhysicSolver:

PhysicSolver& PhysicSolver::add(PhysicBody2d* obj) {
    objects.push_back(obj);
    grid.updateChunkSize(obj);
    return *this;
}

void PhysicSolver::update(const float dtime, const int sub_step) {
    float sub_dt = dtime / sub_step;

    for (int i = 0; i < sub_step; i++) {
        update_acceleration();
        update_constraints();
        update_links();
        grid.assignGrid(objects);
        grid.update_collision();
        //grid.update_collision_mt();//multi_thread
        update_position(sub_dt);
    }
}

void PhysicSolver::update_position(const float dtime)
{
    for (auto& i : objects)
        i->update_position(dtime);
}

void PhysicSolver::update_acceleration() {
    if(acceleration_type == VALUE)
        for (auto& i : objects)
            i->accelerate(accelerationValue);
    else if(acceleration_type == FUNC)
        for (auto& i : objects)
            i->accelerate(acceleration_function(i, objects));
}

void PhysicSolver::update_constraints() {
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
                i->current_position = constratint_fun(i);
    }
}

void PhysicSolver::update_collision() {
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

void PhysicSolver::set_acceleration(const Vec2 accVal) {
    acceleration_type = VALUE;
    accelerationValue = accVal;
}
void PhysicSolver::set_acceleration(std::function<Vec2(PhysicBody2d*, std::vector<PhysicBody2d*>&)> accFun) {
    acceleration_type = FUNC;
    acceleration_function = accFun;
}
void PhysicSolver::set_acceleration() {
    acceleration_type = NONE;
}

void PhysicSolver::set_constraints_def() {
    constraint_type = DEFAULT;
}
void PhysicSolver::set_constraints(std::function<Vec2(PhysicBody2d*)> conFun) {
    constraint_type = FUNC;
    constratint_fun = conFun;
}
void PhysicSolver::set_constraints() {
    constraint_type = NONE;
}

std::pair<bool, PhysicBody2d*> PhysicSolver::pop_from_position(const Vec2& cord) {
    auto found = std::find_if(objects.begin(), objects.end(), [&cord](auto& i) {return i->isHere(cord); });
    if (found != objects.end()) {
        PhysicBody2d *r = * found;
        //objects.erase(found);
        return { true, r };
    }
    return { false, &PhysicBody2d::nullPB };
}

std::pair<bool, PhysicBody2d*> PhysicSolver::get_from_position(const Vec2& cord) {
    auto found = std::find_if(objects.begin(), objects.end(), [&cord](auto& i) {return i->isHere(cord); });
    if (found != objects.end()) {
        PhysicBody2d *r = *found;
        objects.erase(found);
        return { true, r };
    }
    return { false, &PhysicBody2d::nullPB };
}

//PhysicDrawer:

void PhysicDrawer::draw(sf::RenderTarget& target, sf::RenderStates states) const {
    for (const auto& i : physicSolver.objects)
    {
        target.draw(i->getFigure(), states);
    }
    for (const auto& i : physicSolver.links)
    {
        target.draw(i->getFigure(), states);
    }
}