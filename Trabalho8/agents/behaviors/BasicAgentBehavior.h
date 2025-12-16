#pragma once
#include "IAgentBehavior.h"
#include "AStarPathfinder.h"
#include "MoveAgentCommand.h"
#include "CommandProcessor.h"

class BasicAgentBehavior : public IAgentBehavior {
private:
    float speed;
    
public:
    BasicAgentBehavior(float speed = 2.0f) : speed(speed) {}
    
    void Update(Agent& agent, Grid& grid, Vector2& target, std::vector<Vector2>& path, 
               bool& has_path, int& currentPathIndex, float delta_time, CommandProcessor& commandProcessor) override {
        
        if (!has_path) {
            printf("Agent has no path - calling FindPath...\n");
            FindPath(grid, agent.GetPosition(), target, path, has_path, currentPathIndex);
            return;
        }
        
        if (currentPathIndex < path.size()) {
            Vector2 nextCell = path[currentPathIndex];
            Vector2 targetWorldPos = {nextCell.x * grid.GetCellSize() + grid.GetCellSize() / 2, 
                                     nextCell.y * grid.GetCellSize() + grid.GetCellSize() / 2};
            
            Vector2 direction = {targetWorldPos.x - agent.GetPosition().x, targetWorldPos.y - agent.GetPosition().y};
            float distance = sqrt(direction.x * direction.x + direction.y * direction.y);
            
            printf("Moving to point %d/%zu - Distance: %.1f\n", 
                   currentPathIndex, path.size(), distance);
            
            if (distance < 5.0f) {
                currentPathIndex++;
                printf("Reached point, moving to next (%d/%zu)\n", currentPathIndex, path.size());
            } else {
                direction.x /= distance;
                direction.y /= distance;
                
                Vector2 newPosition = {
                    agent.GetPosition().x + direction.x * speed * delta_time * 60.0f,
                    agent.GetPosition().y + direction.y * speed * delta_time * 60.0f
                };

                auto moveCommand = std::make_unique<MoveAgentCommand>(agent, newPosition);
                moveCommand->executionTime = GetTime();
                commandProcessor.AddCommand(std::move(moveCommand));
            }
        } else {
            printf("Reached final destination!\n");
            has_path = false;
        }
    }
    
    void Draw(Grid& grid, Vector2 position, Color color) override {
        DrawCircle(position.x, position.y, grid.GetCellSize() / 3, color);
    }
    
    void FindPath(Grid& grid, Vector2 position, Vector2 target, 
                 std::vector<Vector2>& path, bool& has_path, int& currentPathIndex) override {
        
        printf("=== FINDING PATH ===\n");
        printf("World position: (%.1f, %.1f)\n", position.x, position.y);
        printf("Target: (%.1f, %.1f)\n", target.x, target.y);
        
        AStarPathfinder pathfinder;
        Vector2 gridStart = {position.x / grid.GetCellSize(), position.y / grid.GetCellSize()};
        
        printf("Grid coordinates - Start: (%.1f, %.1f), Target: (%.1f, %.1f)\n",
               gridStart.x, gridStart.y, target.x, target.y);
        
        // Verificar validade das posições
        bool startValid = grid.IsValidPosition((int)gridStart.x, (int)gridStart.y);
        bool targetValid = grid.IsValidPosition((int)target.x, (int)target.y);
        bool startWalkable = startValid ? grid.IsWalkable((int)gridStart.x, (int)gridStart.y) : false;
        bool targetWalkable = targetValid ? grid.IsWalkable((int)target.x, (int)target.y) : false;
        
        printf("Start - valid: %s, walkable: %s\n", 
               startValid ? "YES" : "NO", startWalkable ? "YES" : "NO");
        printf("Target - valid: %s, walkable: %s\n", 
               targetValid ? "YES" : "NO", targetWalkable ? "YES" : "NO");
        
        if (startValid && targetValid && startWalkable && targetWalkable) {
            path = pathfinder.FindPath(grid, gridStart, target, "random");
            has_path = !path.empty();
            printf("Pathfinding result: %s (%zu points)\n", 
                   has_path ? "SUCCESS" : "FAILED", path.size());
        } else {
            printf("ERROR: Cannot find path - invalid positions\n");
            path.clear();
            has_path = false;
        }
        
        currentPathIndex = 0;
        printf("====================\n");
    }
};