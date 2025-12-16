#pragma once
#include "Grid.h"

class IObstacleFactory {
public:
    virtual ~IObstacleFactory() = default;
    virtual void CreateObstacles(Grid& grid, int count) = 0;
    virtual void CreateObstacleAt(Grid& grid, int x, int y) = 0;
};