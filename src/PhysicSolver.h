#pragma once

#include <vector>
#include <algorithm>
#include <functional>
#include <SFML/Graphics.hpp>

#include "PhysicBody2d.h"
#include "PhysicLink2d.h"

#include <omp.h>

class ChunkGrid {
public:
    ChunkGrid(int cS, int wW, int wH);

    void assignGrid(std::vector<PhysicBody2d*>& obj);
    void updateChunkSize(PhysicBody2d* obj);

    ///make sure to reassign grid "assignGrid()", before using
    void update_collision();
    void update_collision_mt();

    void solve_collision(std::vector<PhysicBody2d*>& central, std::vector<PhysicBody2d*>& neigh);

    int count();

    void set_collision_def() { collision_type = DEFAULT; }
    void set_collision() { collision_type = NONE; }
    void set_collision(void (*fun)(PhysicBody2d*, PhysicBody2d*)) { collision_function = fun; collision_type = FUNC; }
    void set_collision(std::function<void(PhysicBody2d*, PhysicBody2d*)> fun) { collision_lambda = fun; collision_type = LAMBDA; }

protected:
    std::vector<std::vector<std::vector<PhysicBody2d*>>> grid;
    int cellSize;
    int grid_width;
    int grid_height;
    int window_width;
    int window_height;

    enum { FUNC, NONE, DEFAULT, LAMBDA } collision_type{ DEFAULT };
    void (*collision_function)(PhysicBody2d*, PhysicBody2d*);
    std::function<void(PhysicBody2d*, PhysicBody2d*)> collision_lambda;

};

class PhysicSolver{
public:
    PhysicSolver(ChunkGrid g) : grid { g } {}
    ~PhysicSolver() {
        for (auto& i : objects) { delete(i); }
        for (auto& i : links) { delete(i); }
    }


    PhysicSolver& add(PhysicBody2d* obj);
    PhysicSolver& addLink(PhysicLink2d* obj) { links.push_back(obj); return *this; }

    void update(const float dtime, const int sub_step = 1);

    void update_position(const float dtime);
    void update_acceleration();
    void update_constraints();
    void update_collision();
    void update_links() {
        for (auto& i : links)
        {
            i->update_link();
        }
    }

    void set_acceleration(const Vec2 accVal);
    void set_acceleration(std::function<Vec2(PhysicBody2d*, std::vector<PhysicBody2d*>&)> accFun);
    void set_acceleration();

    void set_constraints_def();
    void set_constraints(std::function<Vec2(PhysicBody2d*)> conFun);
    void set_constraints();

    std::pair<bool, PhysicBody2d*> pop_from_position(const Vec2& cord);
    std::pair<bool, PhysicBody2d*> get_from_position(const Vec2& cord);

    int getObjectAmount() const { return objects.size(); }
    ChunkGrid& getChunkGrid() { return grid; }
    const std::vector<PhysicBody2d*>& getObjects() const { return objects; }

protected:
    std::vector<PhysicBody2d*> objects{};
    std::vector<PhysicLink2d*> links{};
    ChunkGrid grid;
    friend class PhysicDrawer;

    enum { FUNC, NONE, VALUE, DEFAULT } acceleration_type{ NONE }, constraint_type{ DEFAULT };
    Vec2 accelerationValue;
    std::function<Vec2(PhysicBody2d*, std::vector<PhysicBody2d*>&)> acceleration_function;
    std::function<Vec2(PhysicBody2d*)> constratint_fun;
};

class PhysicDrawer : public sf::Drawable {
public:
    PhysicDrawer(const PhysicSolver& ps) : physicSolver{ ps } {}
    void draw(sf::RenderTarget& target, sf::RenderStates states) const;
protected:
    const PhysicSolver& physicSolver;
};