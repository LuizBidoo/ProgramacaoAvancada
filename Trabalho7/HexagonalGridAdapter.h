#pragma once
#include "IGridAdapter.h"
#include "Grid.h"
#include <vector>
#include <cmath>

class HexagonalGridAdapter : public IGridAdapter {
private:
    Grid& grid;
    
public:
    HexagonalGridAdapter(Grid& grid) : grid(grid) {}
    
    void Draw() override;
    void SetOccupied(int x, int y, bool occupied) override { grid.SetOccupied(x, y, occupied); }
    bool IsWalkable(int x, int y) const override { return grid.IsWalkable(x, y); }
    Node* GetNode(int x, int y) override { return grid.GetNode(x, y); }
    
    std::vector<Node*> GetNeighbors(int x, int y) override;
    void ResetPathfindingData() override { grid.ResetPathfindingData(); }
    
    int GetWidth() const override { return grid.GetWidth(); }
    int GetHeight() const override { return grid.GetHeight(); }
    float GetCellSize() const override { return grid.GetCellSize(); }
};