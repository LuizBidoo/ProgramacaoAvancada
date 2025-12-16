#pragma once
#include "raylib.h"

class Command {
public:
    virtual ~Command() = default;
    virtual void Execute() = 0;
    virtual void Undo() = 0;
    
    double executionTime;
};
