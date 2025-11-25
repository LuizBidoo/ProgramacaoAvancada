#pragma once
#include "raylib.h"
#include "Grid.h"
#include <vector>
#include <string>

class Pathfinder {
public:
    virtual ~Pathfinder() = default;
    virtual std::vector<Vector2> FindPath(Grid& grid, Vector2 start, Vector2 end, 
                                         const std::string& distribution = "random") = 0;
    virtual double GetLastExecutionTime() = 0;
};