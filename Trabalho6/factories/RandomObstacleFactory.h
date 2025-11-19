#pragma once
#include "IObstacleFactory.h"
#include "raylib.h"

class RandomObstacleFactory : public IObstacleFactory {
public:
    void CreateObstacles(Grid& grid, int count) override {
        int width = grid.GetWidth();
        int height = grid.GetHeight();
        
        for (int i = 0; i < count; i++) {
            int x = GetRandomValue(0, width-1);
            int y = GetRandomValue(0, height-1);
            if (GetRandomValue(0, 100) < 25) {
                grid.SetOccupied(x, y, true);
            }
        }
    }
    
    void CreateObstacleAt(Grid& grid, int x, int y) override {
        grid.SetOccupied(x, y, true);
    }
};