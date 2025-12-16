#include "Agent.h"
#include "behaviors/BasicAgentBehavior.h"

Agent::Agent(Vector2 start, Vector2 target, std::unique_ptr<IAgentBehavior> behavior) 
    : position(start), 
      target(target),
      broadRadius(Agent::DEFAULT_BROAD_RADIUS),
      collRadius(Agent::DEFAULT_COLLISION_RADIUS),
      has_path(false), 
      color(GetRandomColor()), 
      currentPathIndex(0) {
    
    if (!behavior) {
        this->behavior = std::make_unique<BasicAgentBehavior>();
    } else {
        this->behavior = std::move(behavior);
    }
    
    //printf("=== AGENT CREATED ===\n");
    //printf("Start world: (%.1f, %.1f)\n", start.x, start.y);
    //printf("Target grid: (%.1f, %.1f)\n", target.x, target.y);
    //printf("=====================\n");
}

void Agent::Update(Grid& grid, float delta_time, CommandProcessor& commandProcessor) {
    behavior->Update(*this, grid, target, path, has_path, currentPathIndex, delta_time, commandProcessor);
}

void Agent::Draw(Grid& grid) {
    behavior->Draw(grid, position, color);
}

Color Agent::GetRandomColor() {
    Color colors[] = {BLUE, PURPLE, ORANGE, PINK, DARKBLUE, DARKPURPLE};
    return colors[GetRandomValue(0, 5)];
}

void Agent::AddObserver(IObserver* observer) {
    observers.push_back(observer);
}

void Agent::RemoveObserver(IObserver* observer) {
    observers.erase(std::remove(observers.begin(), observers.end(), observer), observers.end());
}

void Agent::Notify() {
    for (auto observer : observers) {
        observer->OnNotify(*this);
    }
}