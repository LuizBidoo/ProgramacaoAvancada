#include "AgentManager.h"

void AgentManager::AddAgent(Vector2 start, Vector2 target) {
    Vector2 worldStart = {start.x * grid->GetCellSize() + grid->GetCellSize() / 2, 
                         start.y * grid->GetCellSize() + grid->GetCellSize() / 2};
    agents.emplace_back(worldStart, target);
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

void AgentManager::UpdateAll(float delta_time) {
    for (auto& agent : agents) {
        agent.Update(*grid, delta_time);
    }
}

void AgentManager::DrawAll(Grid& grid) {
    for (auto& agent : agents) {
        agent.Draw(grid);
    }
}