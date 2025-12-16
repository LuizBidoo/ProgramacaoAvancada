#pragma once
#include "raylib.h"
#include "Node.h"
#include <vector>

class IGridAdapter {
public:
    virtual ~IGridAdapter() = default;
    
    virtual void Draw() = 0;
    virtual void SetOccupied(int x, int y, bool occupied) = 0;
    virtual bool IsWalkable(int x, int y) const = 0;
    virtual Node* GetNode(int x, int y) = 0;
    virtual std::vector<Node*> GetNeighbors(int x, int y) = 0;
    virtual void ResetPathfindingData() = 0;
    
    virtual int GetWidth() const = 0;
    virtual int GetHeight() const = 0;
    virtual float GetCellSize() const = 0;
};