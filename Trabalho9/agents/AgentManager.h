#pragma once
#include "Agent.h"
#include "Grid.h"
#include "CommandProcessor.h"
#include "AgentRespawnObserver.h"
#include <vector>
#include <memory>
#include <unordered_map>

class AgentManager {
private:
    static std::unique_ptr<AgentManager> instance;
    std::vector<Agent> agents;
    Grid* grid;
    CommandProcessor commandProcessor;
    std::unique_ptr<AgentRespawnObserver> respawnObserver;
    
public:
    AgentManager(Grid* grid);
    
    AgentManager(const AgentManager&) = delete;
    AgentManager& operator=(const AgentManager&) = delete;
    
    static AgentManager& GetInstance();
    static AgentManager& CreateInstance(Grid* grid);
    static void DestroyInstance();
    
    void AddAgent(Vector2 start, Vector2 target);
    void AddAgentWithBehavior(Vector2 start, Vector2 target, std::unique_ptr<IAgentBehavior> behavior);
    void AddRandomAgents(int count);
    void UpdateAll(float delta_time);
    void DrawAll(Grid& grid);
    int GetAgentCount() const { return agents.size(); }
    std::vector<Agent>& GetAgents() { return agents; }
    CommandProcessor& GetCommandProcessor() { return commandProcessor; }
    void RespawnAgent(Agent& agent);
    void CheckCollision(std::unordered_map<int, int> &collMap, std::unordered_map<int, int> &broadCollMap);
};