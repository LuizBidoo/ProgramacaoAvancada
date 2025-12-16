#pragma once
#include "IAgentBehavior.h"
#include <memory>

class AgentDecorator : public IAgentBehavior {
protected:
    std::unique_ptr<IAgentBehavior> wrappedBehavior;
    
public:
    AgentDecorator(std::unique_ptr<IAgentBehavior> behavior) 
        : wrappedBehavior(std::move(behavior)) {}
    
    void Update(Agent& agent, Grid& grid, Vector2& target, std::vector<Vector2>& path, 
               bool& has_path, int& currentPathIndex, float delta_time, CommandProcessor& commandProcessor) override {
        wrappedBehavior->Update(agent, grid, target, path, has_path, currentPathIndex, delta_time, commandProcessor);
    }
    
    void Draw(Grid& grid, Vector2 position, Color color) override {
        wrappedBehavior->Draw(grid, position, color);
    }
    
    void FindPath(Grid& grid, Vector2 position, Vector2 target, 
                 std::vector<Vector2>& path, bool& has_path, int& currentPathIndex) override {
        wrappedBehavior->FindPath(grid, position, target, path, has_path, currentPathIndex);
    }
};