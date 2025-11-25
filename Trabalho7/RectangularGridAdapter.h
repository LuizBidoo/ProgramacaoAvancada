#pragma once
#include "IGridAdapter.h"
#include "Grid.h"

class RectangularGridAdapter : public IGridAdapter {
private:
    Grid& grid;
    
public:
    RectangularGridAdapter(Grid& grid) : grid(grid) {}
    
    void Draw() override { grid.Draw(); }
    void SetOccupied(int x, int y, bool occupied) override { grid.SetOccupied(x, y, occupied); }
    bool IsWalkable(int x, int y) const override { return grid.IsWalkable(x, y); }
    Node* GetNode(int x, int y) override { return grid.GetNode(x, y); }
    
    std::vector<Node*> GetNeighbors(int x, int y) override {
        std::vector<Node*> neighbors;
        int directions[4][2] = {{0, 1}, {1, 0}, {0, -1}, {-1, 0}};
        
        for (auto& dir : directions) {
            int newX = x + dir[0];
            int newY = y + dir[1];
            
            if (grid.IsWalkable(newX, newY)) {
                Node* neighbor = grid.GetNode(newX, newY);
                if (neighbor) {
                    neighbors.push_back(neighbor);
                }
            }
        }
        return neighbors;
    }
    
    void ResetPathfindingData() override { grid.ResetPathfindingData(); }
    
    int GetWidth() const override { return grid.GetWidth(); }
    int GetHeight() const override { return grid.GetHeight(); }
    float GetCellSize() const override { return grid.GetCellSize(); }
};