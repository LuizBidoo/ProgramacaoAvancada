#pragma once
#include "InitializationHandler.h"
#include "AgentManager.h"
#include "Grid.h"

class AgentManagerInitializationHandler : public InitializationHandler {
private:
    std::unique_ptr<InitializationHandler> nextHandler;

public:
    void SetNext(std::unique_ptr<InitializationHandler> handler) override {
        nextHandler = std::move(handler);
    }

    void Handle() override {
        AgentManager::CreateInstance(&Grid::GetInstance());
        if (nextHandler) {
            nextHandler->Handle();
        }
    }
};
