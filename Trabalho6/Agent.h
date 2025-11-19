#pragma once
#include "raylib.h"
#include "Grid.h"
#include <vector>

class Agent {
private:
    Vector2 position;
    Vector2 target;
    std::vector<Vector2> path;
    bool has_path;
    Color color;
    float speed;
    int currentPathIndex;
    
public:
    Agent(Vector2 start, Vector2 target);
    void Update(Grid& grid, float delta_time);
    void Draw(Grid& grid);
    void FindPath(Grid& grid);
    static Color GetRandomColor();
    bool HasReachedTarget() const { return !has_path && currentPathIndex >= path.size(); }
};