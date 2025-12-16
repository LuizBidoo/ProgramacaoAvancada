#pragma once
#include "AgentDecorator.h"

class SmartPathfindingDecorator : public AgentDecorator {
public:
    SmartPathfindingDecorator(std::unique_ptr<IAgentBehavior> behavior)
        : AgentDecorator(std::move(behavior)) {}
    
    void FindPath(Grid& grid, Vector2 position, Vector2 target, 
                 std::vector<Vector2>& path, bool& has_path, int& currentPathIndex) override {
        //printf("Usando pathfinding inteligente!\n");
        AgentDecorator::FindPath(grid, position, target, path, has_path, currentPathIndex);
    }
};