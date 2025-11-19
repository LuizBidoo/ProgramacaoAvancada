cmake_minimum_required(VERSION 3.16)
project(GridNavigation)

set(CMAKE_CXX_STANDARD 17)

find_package(raylib REQUIRED)

add_executable(GridNavigation
    main.cpp
    Grid.h Grid.cpp
    Node.h
    Pathfinder.h
    AStarPathfinder.h AStarPathfinder.cpp
    Agent.h Agent.cpp
    AgentManager.h AgentManager.cpp
    Metrics.h Metrics.cpp
    factories/IGridFactory.h
    factories/BasicGridFactory.h
    factories/IPathfinderFactory.h
    factories/AStarPathfinderFactory.h
    factories/IAgentFactory.h
    factories/BasicAgentFactory.h
    factories/IObstacleFactory.h
    factories/RandomObstacleFactory.h
    factories/NavigationFactory.h
)

target_link_libraries(GridNavigation raylib)