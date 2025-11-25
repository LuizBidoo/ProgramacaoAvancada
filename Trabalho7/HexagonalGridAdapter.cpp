#include "HexagonalGridAdapter.h"
#include "raylib.h"
#include <cmath>
#include <cstring>

void HexagonalGridAdapter::Draw() {
    float cellSize = grid.GetCellSize();
    int width = grid.GetWidth();
    int height = grid.GetHeight();
    
    float hexWidth = cellSize;
    float hexHeight = cellSize * 0.866f;
    
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            float posX, posY;
            
            if (y % 2 == 0) {
                posX = x * hexWidth;
                posY = y * hexHeight * 0.75f;
            } else {
                posX = x * hexWidth + hexWidth / 2;
                posY = y * hexHeight * 0.75f;
            }
            
            posX += cellSize / 2;
            posY += cellSize / 2;
            
            Color color;
            Node* node = grid.GetNode(x, y);
            if (node) {
                if (node->occupied) {
                    color = RED;
                } else if (!node->walkable) {
                    color = DARKGRAY;
                } else {
                    color = GREEN;
                }
            } else {
                color = GRAY;
            }
            
            DrawPoly((Vector2){posX, posY}, 6, cellSize / 2, 0, color);
            DrawPolyLines((Vector2){posX, posY}, 6, cellSize / 2, 0, LIGHTGRAY);
            
        }
    }
}

std::vector<Node*> HexagonalGridAdapter::GetNeighbors(int x, int y) {
    std::vector<Node*> neighbors;
    
    int directions[6][2];
    
    if (y % 2 == 0) {
        int evenDirections[6][2] = {
            {1, 0}, {0, 1}, {-1, 1}, 
            {-1, 0}, {-1, -1}, {0, -1}
        };
        memcpy(directions, evenDirections, sizeof(evenDirections));
    } else {
        int oddDirections[6][2] = {
            {1, 0}, {1, 1}, {0, 1}, 
            {-1, 0}, {0, -1}, {1, -1}
        };
        memcpy(directions, oddDirections, sizeof(oddDirections));
    }
    
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