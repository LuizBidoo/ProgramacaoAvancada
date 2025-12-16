#pragma once

class Agent;

class IObserver {
public:
    virtual ~IObserver() = default;
    virtual void OnNotify(Agent& agent) = 0;
};
