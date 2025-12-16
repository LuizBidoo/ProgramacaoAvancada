#pragma once
#include "InitializationHandler.h"
#include "Grid.h"

class GridInitializationHandler : public InitializationHandler {
private:
    std::unique_ptr<InitializationHandler> nextHandler;
    int width;
    int height;
    float cellSize;

public:
    GridInitializationHandler(int w, int h, float cs) : width(w), height(h), cellSize(cs) {}

    void SetNext(std::unique_ptr<InitializationHandler> handler) override {
        nextHandler = std::move(handler);
    }

    void Handle() override {
        Grid::CreateInstance(width, height, cellSize);
        if (nextHandler) {
            nextHandler->Handle();
        }
    }
};
