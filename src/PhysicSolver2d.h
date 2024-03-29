#pragma once

#include <iostream>
#include <sstream>
#include <vector>
#include <algorithm>
#include <functional>
#include <SFML/Graphics.hpp>

#include "PhysicBody2d.h"
#include "PhysicLink2d.h"

namespace{
    constexpr int CHUNK_CAPACITY = 4;
}

struct Chunk{
    PhysicBody2d* objects[CHUNK_CAPACITY];
    uint8_t size{ 0 };
    bool isActive{false};
    inline void push_back(PhysicBody2d* obj) {
        if(isFull()){
            // std::cout << "Chunk is full" << std::endl;
            return;
        }
        objects[size] = obj;
        size++;
        if(obj->isKinematic) isActive = true;
    }

    inline void clear() {
        size = 0;
        isActive = false;
    }
    inline bool isFull() const {
        return size == CHUNK_CAPACITY;
    }
    inline bool canAdd() const {
        return !size;
    }

    PhysicBody2d* operator[](int i) {
        return objects[i];
    }
    const PhysicBody2d* operator[](int i) const {
        return objects[i];
    }

    PhysicBody2d** begin() {
        return objects;
    }
    PhysicBody2d* const * begin() const {
        return objects;
    }
    PhysicBody2d** end() {
        return objects + size;
    }
    const PhysicBody2d* const * end() const {
        return objects + size;
    }
    inline bool empty() const {
        return size == 0;
    }
};

class ChunkGrid {
public:
    ChunkGrid(int cS, int wW, int wH);

    void assignGrid(const std::vector<PhysicBody2d*>& obj);
    void updateChunkSize(const PhysicBody2d* obj);

    ///make sure to reassign grid "assignGrid()", before using
    void update_collision();
    void update_collision_mt();

    void solve_collision(Chunk& central, Chunk& neigh);

    int count();

    void set_collision_def() { collision_type = solvingMethod::DEFAULT; }
    void set_collision() { collision_type = solvingMethod::NONE; }
    void set_collision(void (*fun)(PhysicBody2d*, PhysicBody2d*)) { collision_function = fun; collision_type = solvingMethod::FUNC; }
    void set_collision(std::function<void(PhysicBody2d*, PhysicBody2d*)> fun) { collision_lambda = fun; collision_type = solvingMethod::LAMBDA; }

    std::vector<Chunk>& getGrid() { return grid; }
    const std::vector<Chunk>& getGrid() const { return grid; }

    int getChunkSize() const { return cellSize; }
    int getWidth() const { return grid_width; }
    int getHeight() const { return grid_height; }
    Chunk& getChunk(int x, int y) { return grid.at(x + y * grid_width); }

protected:
    std::vector<Chunk> grid;
    int cellSize;
    int grid_width;
    int grid_height;
    int window_width;
    int window_height;

    enum class solvingMethod { FUNC, NONE, DEFAULT, LAMBDA } collision_type{ solvingMethod::DEFAULT };
    void (*collision_function)(PhysicBody2d*, PhysicBody2d*);
    std::function<void(PhysicBody2d*, PhysicBody2d*)> collision_lambda;

};

class PhysicSolver2d{
public:
    PhysicSolver2d(ChunkGrid g) : grid { g } {}
    ~PhysicSolver2d() {
        for (auto& i : objects) { delete(i); }
        for (auto& i : links) { delete(i); }
    }


    PhysicSolver2d& add(PhysicBody2d* obj); //deprecated
    PhysicSolver2d& add(Vec2 position, float size, bool isKinematic = false, sf::Color color = sf::Color::White);
    PhysicSolver2d& insert(Vec2 position, float size, bool isKinematic = false, sf::Color color = sf::Color::White);
    PhysicSolver2d& addLink(PhysicBody2d* obj1, PhysicBody2d* obj2, float len);

    void update(const float d_time, const int sub_step = 1);

    void update_position(const float d_time);
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

    size_t getObjectAmount() const { return objects.size(); }
    ChunkGrid& getChunkGrid() { return grid; }
    const ChunkGrid& getChunkGrid() const { return grid; }
    const std::vector<PhysicBody2d*>& getObjects() const { return objects; }
    void deleteObject(PhysicBody2d* obj);
    void clear();

    long long getSimResult(const size_t i);

protected:
    std::vector<PhysicBody2d*> objects{};
    std::vector<PhysicLink2d*> links{};
    ChunkGrid grid;
    friend class PhysicDrawer;

    enum { FUNC, NONE, VALUE, DEFAULT } acceleration_type{ NONE }, constraint_type{ DEFAULT };
    Vec2 accelerationValue;
    std::function<Vec2(PhysicBody2d*, std::vector<PhysicBody2d*>&)> acceleration_function;
    std::function<Vec2(PhysicBody2d*)> constraint_fun;
    long long simResult[6];
};

class PhysicDrawer : public sf::Drawable {
public:
    PhysicDrawer(const PhysicSolver2d& pS, const Vec2 wS, const float cS);
    void draw(sf::RenderTarget& target, sf::RenderStates states) const;
protected:
    const PhysicSolver2d& physicSolver;
    const Vec2 windowSize;
    const float particleSize;
};