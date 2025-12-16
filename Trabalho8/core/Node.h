#pragma once
#include "raylib.h"

struct Node {
public:
    int x, y;
    bool occupied;
    bool walkable;
    
    float gCost;
    float hCost;
    float fCost() const { return gCost + hCost; }
    
    Node* parent;
    
    Node() : x(0), y(0), occupied(false), walkable(true), 
             gCost(0), hCost(0), parent(nullptr) {}
    
    Node(int x, int y) : x(x), y(y), occupied(false), walkable(true), 
                         gCost(0), hCost(0), parent(nullptr) {}
    
    bool operator==(const Node& other) const {
        return x == other.x && y == other.y;
    }
};