#pragma once
#include "../Pathfinder.h"
#include <memory>

class IPathfinderFactory {
public:
    virtual ~IPathfinderFactory() = default;
    virtual std::unique_ptr<Pathfinder> CreatePathfinder() = 0;
};