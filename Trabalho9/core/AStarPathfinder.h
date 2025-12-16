#pragma once
#include "Pathfinder.h"
#include "Grid.h"
#include <vector>
#include <algorithm>
#include <cmath>
#include <functional>
#include <queue>

class AStarPathfinder : public Pathfinder {
private:
    static double lastExecutionTime;
    
    float CalculateHeuristic(int x1, int y1, int x2, int y2);
    std::vector<Node*> GetNeighbors(Node* node, Grid& grid);
    std::vector<Vector2> ReconstructPath(Node* endNode);
    
public:
    std::vector<Vector2> FindPath(Grid& grid, Vector2 start, Vector2 end, 
                                 const std::string& distribution = "random") override;
    double GetLastExecutionTime() override { return lastExecutionTime; }
};