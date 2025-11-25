#pragma once
#include "raylib.h"
#include "Grid.h"
#include <vector>

class IAgentBehavior {
public:
    virtual ~IAgentBehavior() = default;
    
    virtual void Update(Grid& grid, Vector2& position, Vector2& target, std::vector<Vector2>& path, 
                       bool& has_path, int& currentPathIndex, float delta_time) = 0;
    virtual void Draw(Grid& grid, Vector2 position, Color color) = 0;
    virtual void FindPath(Grid& grid, Vector2 position, Vector2 target, 
                         std::vector<Vector2>& path, bool& has_path, int& currentPathIndex) = 0;
};