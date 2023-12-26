// #include "PhysicSolver.h"
#include "Selector.hpp"

Selector::Selector(ChunkGrid& cg) : chunkGrid{ cg } {}
Selector::~Selector() = default;

void Selector::select(Vec2 pos, float radius) {
    selected.clear();
    int chunkSize = chunkGrid.getChunkSize();
    int x = int(pos.x / chunkSize);
    int y = int(pos.y / chunkSize);
    int r = int(radius / chunkSize + 1);
    for (int i = x - r; i <= x + r; i++) {
        for (int j = y - r; j <= y + r; j++) {
            if (i < 0 || j < 0 || i >= chunkGrid.getWidth() || j >= chunkGrid.getHeight()) continue;
            Chunk& chunk = chunkGrid.getChunk(i, j);
            for (PhysicBody2d* obj : chunk) {
                if ((obj->getPos() - pos).length() < radius) {
                    selected.push_back(obj);
                }
            }
        }
    }
}