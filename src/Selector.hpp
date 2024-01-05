#include <vector>

#include "PhysicSolver2d.h"


class Selector {
public:
    Selector(ChunkGrid& cg);// : chunkGrid{ cg } {}
    ~Selector();
    void select(Vec2 pos, float radius);
    std::vector<PhysicBody2d*>& getSelected() { return selected; }

private:
    std::vector<PhysicBody2d*> selected;
    ChunkGrid& chunkGrid;
};