#include "AgentRespawnObserver.h"
#include "AgentManager.h"
#include "Agent.h"

AgentRespawnObserver::AgentRespawnObserver(AgentManager& am) : agentManager(am) {}

void AgentRespawnObserver::OnNotify(Agent& agent) {
    if (agent.GetLife() <= 0) {
        //printf("Agent died, respawning...\n");
        agentManager.RespawnAgent(agent);
    }
}

