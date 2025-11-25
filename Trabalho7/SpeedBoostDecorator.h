#pragma once
#include "AgentDecorator.h"

class SpeedBoostDecorator : public AgentDecorator {
private:
    float speedMultiplier;
    
public:
    SpeedBoostDecorator(std::unique_ptr<IAgentBehavior> behavior, float multiplier = 1.5f)
        : AgentDecorator(std::move(behavior)), speedMultiplier(multiplier) {}
    
    void Update(Grid& grid, Vector2& position, Vector2& target, std::vector<Vector2>& path, 
               bool& has_path, int& currentPathIndex, float delta_time) override {
        AgentDecorator::Update(grid, position, target, path, has_path, currentPathIndex, delta_time * speedMultiplier);
    }
};