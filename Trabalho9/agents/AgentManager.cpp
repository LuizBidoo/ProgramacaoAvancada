#include "AgentManager.h"
#include "behaviors/BasicAgentBehavior.h"
#include "raylib.h"
#include <unordered_map>

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
    Vector2 start;
    do {
        start = {(float)GetRandomValue(0, grid->GetWidth() - 1), 
                (float)GetRandomValue(0, grid->GetHeight() - 1)};
    } while (!grid->IsWalkable((int)start.x, (int)start.y));
    
    Vector2 worldStart = {start.x * grid->GetCellSize() + grid->GetCellSize() / 2, 
                         start.y * grid->GetCellSize() + grid->GetCellSize() / 2};
    agent.SetPosition(worldStart);
}

void AgentManager::CheckCollision(std::unordered_map<int, int> &collMap, std::unordered_map<int, int> &broadCollMap) {
    for(size_t i = 0; i < agents.size(); i++) {
        for(size_t j = i + 1; j < agents.size(); j++) {
            float dx = agents[i].GetPosition().x - agents[j].GetPosition().x;
            float dy = agents[i].GetPosition().y - agents[j].GetPosition().y;
            float distSq = dx * dx + dy * dy;

            float broadRadiusSum = agents[i].DEFAULT_BROAD_RADIUS + agents[j].DEFAULT_BROAD_RADIUS;
            float broadRadiusSumSq = broadRadiusSum * broadRadiusSum;

            if(distSq <= broadRadiusSumSq) {
                broadCollMap.insert({i, j});
                printf("Colisão maior detectada entre %d e %d\n\n", i, j);
                float collisionRadiusSum = agents[i].DEFAULT_COLLISION_RADIUS + agents[j].DEFAULT_COLLISION_RADIUS;
                float collisionRadiusSumSq = collisionRadiusSum * collisionRadiusSum;
                if(distSq <= collisionRadiusSumSq) {
                    collMap.insert({i, j});
                    printf("Colisão menor detectada entre %d e %d\n\n", i, j);
                }
            }
        }
    }
}

