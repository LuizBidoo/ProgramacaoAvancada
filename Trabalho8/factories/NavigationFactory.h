#pragma once
#include "IGridFactory.h"
#include "IPathfinderFactory.h"
#include "IAgentFactory.h"
#include "IObstacleFactory.h"
#include <memory>

class NavigationFactory {
private:
    std::unique_ptr<IGridFactory> gridFactory;
    std::unique_ptr<IPathfinderFactory> pathfinderFactory;
    std::unique_ptr<IAgentFactory> agentFactory;
    std::unique_ptr<IObstacleFactory> obstacleFactory;
    
public:
    NavigationFactory(std::unique_ptr<IGridFactory> gridF,
                     std::unique_ptr<IPathfinderFactory> pathF,
                     std::unique_ptr<IAgentFactory> agentF,
                     std::unique_ptr<IObstacleFactory> obstacleF)
        : gridFactory(std::move(gridF))
        , pathfinderFactory(std::move(pathF))
        , agentFactory(std::move(agentF))
        , obstacleFactory(std::move(obstacleF)) {}
    
    std::unique_ptr<Grid> CreateGrid(int w, int h, float cellSize) {
        return gridFactory->CreateGrid(w, h, cellSize);
    }
    
    std::unique_ptr<Pathfinder> CreatePathfinder() {
        return pathfinderFactory->CreatePathfinder();
    }
    
    std::unique_ptr<Agent> CreateAgent(Vector2 start, Vector2 target) {
        return agentFactory->CreateAgent(start, target);
    }
    
    std::unique_ptr<AgentManager> CreateAgentManager(Grid* grid) {
        return agentFactory->CreateAgentManager(grid);
    }
    
    void CreateObstacles(Grid& grid, int count) {
        obstacleFactory->CreateObstacles(grid, count);
    }
    
    void CreateObstacleAt(Grid& grid, int x, int y) {
        obstacleFactory->CreateObstacleAt(grid, x, y);
    }
};