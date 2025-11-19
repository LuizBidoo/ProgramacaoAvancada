#include "Agent.h"
#include "AStarPathfinder.h"

Agent::Agent(Vector2 start, Vector2 target) : position(start), target(target), has_path(false), 
                                              color(GetRandomColor()), speed(2.0f), currentPathIndex(0) {}

void Agent::Update(Grid& grid, float delta_time) {
    if (!has_path) {
        FindPath(grid);
        return;
    }
    
    if (currentPathIndex < path.size()) {
        Vector2 nextCell = path[currentPathIndex];
        Vector2 targetWorldPos = {nextCell.x * grid.GetCellSize() + grid.GetCellSize() / 2, 
                                 nextCell.y * grid.GetCellSize() + grid.GetCellSize() / 2};
        
        Vector2 direction = {targetWorldPos.x - position.x, targetWorldPos.y - position.y};
        float distance = sqrt(direction.x * direction.x + direction.y * direction.y);
        
        if (distance < 5.0f) {
            currentPathIndex++;
        } else {
            direction.x /= distance;
            direction.y /= distance;
            position.x += direction.x * speed * delta_time * 60.0f;
            position.y += direction.y * speed * delta_time * 60.0f;
        }
    } else {
        has_path = false;
    }
}

void Agent::Draw(Grid& grid) {
    DrawCircle(position.x, position.y, grid.GetCellSize() / 3, color);
}

void Agent::FindPath(Grid& grid) {
    AStarPathfinder pathfinder;
    Vector2 gridStart = {position.x / grid.GetCellSize(), position.y / grid.GetCellSize()};
    path = pathfinder.FindPath(grid, gridStart, target, "random");
    has_path = !path.empty();
    currentPathIndex = 0;
}

Color Agent::GetRandomColor() {
    Color colors[] = {BLUE, PURPLE, ORANGE, PINK, DARKBLUE, DARKPURPLE};
    return colors[GetRandomValue(0, 5)];
}