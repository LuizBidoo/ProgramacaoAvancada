#pragma once
#include "AgentDecorator.h"

class SpeedBoostDecorator : public AgentDecorator {
private:
    float speedMultiplier;
    
public:
    SpeedBoostDecorator(std::unique_ptr<IAgentBehavior> behavior, float multiplier = 1.5f)
        : AgentDecorator(std::move(behavior)), speedMultiplier(multiplier) {}
    
    void Update(Agent& agent, Grid& grid, Vector2& target, std::vector<Vector2>& path, 
               bool& has_path, int& currentPathIndex, float delta_time, CommandProcessor& commandProcessor) override {
        AgentDecorator::Update(agent, grid, target, path, has_path, currentPathIndex, delta_time * speedMultiplier, commandProcessor);
    }
};