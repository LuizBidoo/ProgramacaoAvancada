#pragma once
#include "raylib.h"
#include "Grid.h"
#include "IAgentBehavior.h"
#include <vector>
#include <memory>

class Agent {
private:
    Vector2 position;
    Vector2 target;
    std::vector<Vector2> path;
    bool has_path;
    Color color;
    int currentPathIndex;
    
    std::unique_ptr<IAgentBehavior> behavior;
    
public:
    Agent(Vector2 start, Vector2 target, std::unique_ptr<IAgentBehavior> behavior = nullptr);
    
    void Update(Grid& grid, float delta_time);
    void Draw(Grid& grid);
    static Color GetRandomColor();
    bool HasReachedTarget() const { return !has_path && currentPathIndex >= path.size(); }
    
    void SetBehavior(std::unique_ptr<IAgentBehavior> newBehavior) {
        behavior = std::move(newBehavior);
    }
};