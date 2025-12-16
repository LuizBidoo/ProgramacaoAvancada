#pragma once
#include "raylib.h"
#include "Grid.h"
#include <vector>

class CommandProcessor;
class Agent;

class IAgentBehavior {
public:
    virtual ~IAgentBehavior() = default;
    
    virtual void Update(Agent& agent, Grid& grid, Vector2& target, std::vector<Vector2>& path, 
                       bool& has_path, int& currentPathIndex, float delta_time, CommandProcessor& commandProcessor) = 0;
    virtual void Draw(Grid& grid, Vector2 position, Color color) = 0;
    virtual void FindPath(Grid& grid, Vector2 position, Vector2 target, 
                         std::vector<Vector2>& path, bool& has_path, int& currentPathIndex) = 0;
};