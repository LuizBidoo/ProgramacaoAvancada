#pragma once
#include "IGridFactory.h"

class BasicGridFactory : public IGridFactory {
public:
    std::unique_ptr<Grid> CreateGrid(int width, int height, float cellSize) override {
        return std::make_unique<Grid>(width, height, cellSize);
    }
};