#pragma once

#include "PhysicSolver2d.h"

class GameLogic
{
private:
    PhysicSolver2d& sandbox;
public:
    GameLogic(PhysicSolver2d& sandbox) : sandbox(sandbox) {}
    void update() {
    }
    ~GameLogic() = default;
};
