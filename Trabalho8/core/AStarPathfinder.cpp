#include "AStarPathfinder.h"

double AStarPathfinder::lastExecutionTime = 0.0;

float AStarPathfinder::CalculateHeuristic(int x1, int y1, int x2, int y2) {
    return abs(x1 - x2) + abs(y1 - y2);
}

std::vector<Node*> AStarPathfinder::GetNeighbors(Node* node, Grid& grid) {
    std::vector<Node*> neighbors;
    int directions[4][2] = {{0, 1}, {1, 0}, {0, -1}, {-1, 0}};
    
    for (auto& dir : directions) {
        int newX = node->x + dir[0];
        int newY = node->y + dir[1];
        
        if (grid.IsWalkable(newX, newY)) {
            Node* neighbor = grid.GetNode(newX, newY);
            if (neighbor) {
                neighbors.push_back(neighbor);
            }
        }
    }
    return neighbors;
}

std::vector<Vector2> AStarPathfinder::ReconstructPath(Node* endNode) {
    std::vector<Vector2> path;
    Node* currentNode = endNode;
    
    while (currentNode != nullptr) {
        path.push_back({(float)currentNode->x, (float)currentNode->y});
        currentNode = currentNode->parent;
    }
    
    std::reverse(path.begin(), path.end());
    return path;
}

std::vector<Vector2> AStarPathfinder::FindPath(Grid& grid, Vector2 start, Vector2 end, const std::string& distribution) {
    double startTime = GetTime();
    
    grid.ResetPathfindingData();
    
    Node* startNode = grid.GetNode((int)start.x, (int)start.y);
    Node* endNode = grid.GetNode((int)end.x, (int)end.y);
    
    if (!startNode || !endNode || !startNode->walkable || !endNode->walkable) {
        return {};
    }
    
    std::vector<Node*> openSet;
    std::vector<Node*> closedSet;
    
    openSet.push_back(startNode);
    
    while (!openSet.empty()) {
        Node* currentNode = openSet[0];
        int currentIndex = 0;
        
        for (int i = 1; i < openSet.size(); i++) {
            if (openSet[i]->fCost() < currentNode->fCost() || 
                (openSet[i]->fCost() == currentNode->fCost() && openSet[i]->hCost < currentNode->hCost)) {
                currentNode = openSet[i];
                currentIndex = i;
            }
        }
        
        openSet.erase(openSet.begin() + currentIndex);
        closedSet.push_back(currentNode);
        
        if (currentNode == endNode) {
            lastExecutionTime = GetTime() - startTime;
            return ReconstructPath(currentNode);
        }
        
        auto neighbors = GetNeighbors(currentNode, grid);
        for (auto neighbor : neighbors) {
            if (std::find(closedSet.begin(), closedSet.end(), neighbor) != closedSet.end()) {
                continue;
            }
            
            float newGCost = currentNode->gCost + 1;
            
            if (newGCost < neighbor->gCost || 
                std::find(openSet.begin(), openSet.end(), neighbor) == openSet.end()) {
                
                neighbor->gCost = newGCost;
                neighbor->hCost = CalculateHeuristic(neighbor->x, neighbor->y, endNode->x, endNode->y);
                neighbor->parent = currentNode;
                
                if (std::find(openSet.begin(), openSet.end(), neighbor) == openSet.end()) {
                    openSet.push_back(neighbor);
                }
            }
        }
    }
    
    lastExecutionTime = GetTime() - startTime;
    return {};
}