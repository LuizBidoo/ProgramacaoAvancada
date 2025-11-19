#pragma once
#include "raylib.h"
#include "Node.h"
#include <vector>

class Grid {
private:
    int width, height;
    float cell_size;
    std::vector<std::vector<Node>> nodes;
    
public:
    Grid(int w, int h, float cell_size);
    virtual ~Grid() = default;
    
    virtual void Draw();
    virtual void SetOccupied(int x, int y, bool occupied);
    virtual void SetWalkable(int x, int y, bool walkable);
    virtual bool IsValidPosition(int x, int y) const;
    virtual bool IsWalkable(int x, int y) const;
    virtual Node* GetNode(int x, int y);
    virtual void ResetPathfindingData();
    
    int GetWidth() const { return width; }
    int GetHeight() const { return height; }
    float GetCellSize() const { return cell_size; }
};