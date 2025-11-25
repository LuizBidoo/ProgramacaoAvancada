#pragma once
#include "Agent.h"
#include "Grid.h"
#include <vector>
#include <memory>

class AgentManager {
private:
    static std::unique_ptr<AgentManager> instance;
    std::vector<Agent> agents;
    Grid* grid;
    
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
};