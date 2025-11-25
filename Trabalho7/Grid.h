#pragma once
#include "raylib.h"
#include "Node.h"
#include <vector>
#include <memory>

class Grid {
private:
    static std::unique_ptr<Grid> instance;
    
    int width, height;
    float cell_size;
    std::vector<std::vector<Node>> nodes;
    
public:
    Grid(int w, int h, float cell_size);
    
    Grid(const Grid&) = delete;
    Grid& operator=(const Grid&) = delete;
    
    static Grid& GetInstance();
    static Grid& CreateInstance(int w, int h, float cell_size);
    static void DestroyInstance();
    
    void Draw();
    void SetOccupied(int x, int y, bool occupied);
    void SetWalkable(int x, int y, bool walkable);
    bool IsValidPosition(int x, int y) const;
    bool IsWalkable(int x, int y) const;
    Node* GetNode(int x, int y);
    void ResetPathfindingData();
    
    int GetWidth() const { return width; }
    int GetHeight() const { return height; }
    float GetCellSize() const { return cell_size; }
};