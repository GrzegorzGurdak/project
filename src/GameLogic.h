#pragma once

#include "PhysicSolver.h"

class GameLogic
{
private:
    PhysicSolver& sandbox;
public:
    GameLogic(PhysicSolver& sandbox) : sandbox(sandbox) {}
    void update() {
    }
    ~GameLogic() = default;
};
