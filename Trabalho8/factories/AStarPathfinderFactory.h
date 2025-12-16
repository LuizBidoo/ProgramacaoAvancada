#pragma once
#include "IPathfinderFactory.h"
#include "AStarPathfinder.h"

class AStarPathfinderFactory : public IPathfinderFactory {
public:
    std::unique_ptr<Pathfinder> CreatePathfinder() override {
        return std::make_unique<AStarPathfinder>();
    }
};