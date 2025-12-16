#pragma once
#include "Agent.h"
#include "AgentManager.h"
#include <memory>

class IAgentFactory {
public:
    virtual ~IAgentFactory() = default;
    virtual std::unique_ptr<Agent> CreateAgent(Vector2 start, Vector2 target) = 0;
    virtual std::unique_ptr<AgentManager> CreateAgentManager(Grid* grid) = 0;
};