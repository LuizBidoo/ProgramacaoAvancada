#pragma once
#include "Command.h"
#include "Agent.h"

class MoveAgentCommand : public Command {
private:
    Agent& agent;
    Vector2 previousPosition;
    Vector2 newPosition;

public:
    MoveAgentCommand(Agent& agent, Vector2 newPosition)
        : agent(agent), newPosition(newPosition) {
        previousPosition = agent.GetPosition();
    }

    void Execute() override {
        agent.SetPosition(newPosition);
    }

    void Undo() override {
        agent.SetPosition(previousPosition);
    }
};
