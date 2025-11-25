#pragma once
#include "../Grid.h"
#include <memory>

class IGridFactory {
public:
    virtual ~IGridFactory() = default;
    virtual std::unique_ptr<Grid> CreateGrid(int width, int height, float cellSize) = 0;
};