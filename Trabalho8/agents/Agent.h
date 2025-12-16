#pragma once
#include "raylib.h"
#include "Grid.h"
#include "behaviors/IAgentBehavior.h"
#include "ISubject.h"
#include <vector>
#include <memory>

class Agent : public ISubject {
private:
    Vector2 position;
    Vector2 target;
    std::vector<Vector2> path;
    bool has_path;
    Color color;
    int currentPathIndex;
    float life = 100.0f;
    
    std::unique_ptr<IAgentBehavior> behavior;
    std::vector<IObserver*> observers;
    
public:
    Agent(Vector2 start, Vector2 target, std::unique_ptr<IAgentBehavior> behavior = nullptr);
    
    void Update(Grid& grid, float delta_time, CommandProcessor& commandProcessor);
    void Draw(Grid& grid);
    static Color GetRandomColor();
    bool HasReachedTarget() const { return !has_path && currentPathIndex >= path.size(); }
    
    void SetBehavior(std::unique_ptr<IAgentBehavior> newBehavior) {
        behavior = std::move(newBehavior);
    }

    Vector2 GetPosition() const { return position; }
    void SetPosition(Vector2 newPosition) { position = newPosition; }

    void AddObserver(IObserver* observer) override;
    void RemoveObserver(IObserver* observer) override;
    void Notify() override;

    void TakeDamage(float damage) {
        life -= damage;
        if (life <= 0) {
            Notify();
        }
    }

    float GetLife() const { return life; }
};