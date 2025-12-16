#include "AgentManager.h"
#include "behaviors/BasicAgentBehavior.h"
#include "raylib.h"

std::unique_ptr<AgentManager> AgentManager::instance = nullptr;

AgentManager::AgentManager(Grid* grid) : grid(grid) {
    respawnObserver = std::make_unique<AgentRespawnObserver>(*this);
}

AgentManager& AgentManager::GetInstance() {
    if (!instance) {
        instance = std::make_unique<AgentManager>(&Grid::GetInstance());
    }
    return *instance;
}

AgentManager& AgentManager::CreateInstance(Grid* grid) {
    if (!instance) {
        instance = std::make_unique<AgentManager>(grid);
    }
    return *instance;
}

void AgentManager::DestroyInstance() {
    instance.reset();
}

void AgentManager::AddAgent(Vector2 start, Vector2 target) {
    Vector2 worldStart = {start.x * grid->GetCellSize() + grid->GetCellSize() / 2, 
                         start.y * grid->GetCellSize() + grid->GetCellSize() / 2};
    agents.emplace_back(worldStart, target);
    agents.back().AddObserver(respawnObserver.get());
}

void AgentManager::AddRandomAgents(int count) {
    for (int i = 0; i < count; i++) {
        Vector2 start, target;
        
        do {
            start = {(float)GetRandomValue(0, grid->GetWidth() - 1), 
                    (float)GetRandomValue(0, grid->GetHeight() - 1)};
        } while (!grid->IsWalkable((int)start.x, (int)start.y));
        
        do {
            target = {(float)GetRandomValue(0, grid->GetWidth() - 1), 
                     (float)GetRandomValue(0, grid->GetHeight() - 1)};
        } while (!grid->IsWalkable((int)target.x, (int)target.y) || 
                (start.x == target.x && start.y == target.y));
        
        AddAgent(start, target);
    }
}

void AgentManager::AddAgentWithBehavior(Vector2 start, Vector2 target, std::unique_ptr<IAgentBehavior> behavior) {
    Vector2 worldStart = {start.x * grid->GetCellSize() + grid->GetCellSize() / 2, 
                         start.y * grid->GetCellSize() + grid->GetCellSize() / 2};
    agents.emplace_back(worldStart, target, std::move(behavior));
    agents.back().AddObserver(respawnObserver.get());
}

void AgentManager::UpdateAll(float delta_time) {
    for (auto& agent : agents) {
        agent.Update(*grid, delta_time, commandProcessor);
    }
    commandProcessor.ProcessCommands();
}

void AgentManager::DrawAll(Grid& grid) {
    for (auto& agent : agents) {
        agent.Draw(grid);
    }
}

void AgentManager::RespawnAgent(Agent& agent) {
    // For simplicity, we'll just reset the agent's position to a random one.
    Vector2 start;
    do {
        start = {(float)GetRandomValue(0, grid->GetWidth() - 1), 
                (float)GetRandomValue(0, grid->GetHeight() - 1)};
    } while (!grid->IsWalkable((int)start.x, (int)start.y));
    
    Vector2 worldStart = {start.x * grid->GetCellSize() + grid->GetCellSize() / 2, 
                         start.y * grid->GetCellSize() + grid->GetCellSize() / 2};
    agent.SetPosition(worldStart);
}