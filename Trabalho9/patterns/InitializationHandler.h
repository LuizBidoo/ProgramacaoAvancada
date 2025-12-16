#pragma once
#include <memory>

class InitializationHandler {
public:
    virtual ~InitializationHandler() = default;
    virtual void SetNext(std::unique_ptr<InitializationHandler> handler) = 0;
    virtual void Handle() = 0;
};
