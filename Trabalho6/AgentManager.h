#pragma once
#include "Agent.h"
#include "Grid.h"
#include <vector>
#include <memory>

class AgentManager {
private:
    std::vector<Agent> agents;
    Grid* grid;
    
public:
    AgentManager(Grid* grid) : grid(grid) {}
    
    void AddAgent(Vector2 start, Vector2 target);
    void AddRandomAgents(int count);
    void UpdateAll(float delta_time);
    void DrawAll(Grid& grid);
    int GetAgentCount() const { return agents.size(); }
};