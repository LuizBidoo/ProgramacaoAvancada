#pragma once
#include "IObserver.h"
#include "Agent.h"
#include <cstdio>

class AgentManager;

class AgentRespawnObserver : public IObserver {
private:
    AgentManager& agentManager;

public:
    AgentRespawnObserver(AgentManager& am);

    void OnNotify(Agent& agent) override;
};

