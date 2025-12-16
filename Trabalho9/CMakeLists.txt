cmake_minimum_required(VERSION 3.16)
project(GridNavigation)

set(CMAKE_CXX_STANDARD 17)

find_package(raylib REQUIRED)

add_executable(GridNavigation
    core/main.cpp
    core/Grid.cpp
    core/AStarPathfinder.cpp
    core/Metrics.cpp
    agents/Agent.cpp
    agents/AgentManager.cpp
    grids/HexagonalGridAdapter.cpp
    patterns/AgentRespawnObserver.cpp
)

target_include_directories(GridNavigation PUBLIC
    agents
    agents/behaviors
    core
    factories
    grids
    patterns
)

target_link_libraries(GridNavigation raylib)
