#pragma once
#include "IObserver.h"
#include <vector>
#include <memory>

class ISubject {
public:
    virtual ~ISubject() = default;
    virtual void AddObserver(IObserver* observer) = 0;
    virtual void RemoveObserver(IObserver* observer) = 0;
    virtual void Notify() = 0;
};
