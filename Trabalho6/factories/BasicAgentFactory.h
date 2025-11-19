#pragma once
#include "IAgentFactory.h"

class BasicAgentFactory : public IAgentFactory {
public:
    std::unique_ptr<Agent> CreateAgent(Vector2 start, Vector2 target) override {
        return std::make_unique<Agent>(start, target);
    }
    
    std::unique_ptr<AgentManager> CreateAgentManager(Grid* grid) override {
        return std::make_unique<AgentManager>(grid);
    }
};