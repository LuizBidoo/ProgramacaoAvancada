#pragma once
#include "Command.h"
#include <vector>
#include <memory>

class CommandProcessor {
private:
    std::vector<std::unique_ptr<Command>> commandQueue;
    std::vector<std::unique_ptr<Command>> commandHistory;

public:
    void AddCommand(std::unique_ptr<Command> command) {
        commandQueue.push_back(std::move(command));
    }

    void ProcessCommands() {
        double currentTime = GetTime();
        for (auto it = commandQueue.begin(); it != commandQueue.end(); ) {
            if (currentTime >= (*it)->executionTime) {
                (*it)->Execute();
                commandHistory.push_back(std::move(*it));
                it = commandQueue.erase(it);
            } else {
                ++it;
            }
        }
    }

    void UndoLastCommand() {
        if (!commandHistory.empty()) {
            commandHistory.back()->Undo();
            commandHistory.pop_back();
        }
    }
};
