#include "Grid.h"

std::unique_ptr<Grid> Grid::instance = nullptr;

Grid::Grid(int w, int h, float cell_size) : width(w), height(h), cell_size(cell_size) {
    nodes.resize(height);
    for (int y = 0; y < height; y++) {
        nodes[y].resize(width);
        for (int x = 0; x < width; x++) {
            nodes[y][x] = Node(x, y); 
        }
    }
}

Grid& Grid::GetInstance() {
    if (!instance) {
        instance = std::make_unique<Grid>(10, 10, 60.0f);
    }
    return *instance;
}

Grid& Grid::CreateInstance(int w, int h, float cell_size) {
    if (!instance) {
        instance = std::make_unique<Grid>(w, h, cell_size);
    }
    return *instance;
}

void Grid::DestroyInstance() {
    instance.reset();
}

void Grid::Draw() {
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            Color color = nodes[y][x].occupied ? RED : GREEN;
            if (!nodes[y][x].walkable) color = DARKGRAY;
            
            DrawRectangle(x * cell_size, y * cell_size, cell_size - 1, cell_size - 1, color);
            DrawRectangleLines(x * cell_size, y * cell_size, cell_size, cell_size, LIGHTGRAY);
        }
    }
}

void Grid::SetOccupied(int x, int y, bool occupied) {
    if (IsValidPosition(x, y)) {
        nodes[y][x].occupied = occupied;
        nodes[y][x].walkable = !occupied;
    }
}

void Grid::SetWalkable(int x, int y, bool walkable) {
    if (IsValidPosition(x, y)) {
        nodes[y][x].walkable = walkable;
        nodes[y][x].occupied = !walkable;
    }
}

bool Grid::IsValidPosition(int x, int y) const {
    return x >= 0 && x < width && y >= 0 && y < height;
}

bool Grid::IsWalkable(int x, int y) const {
    return IsValidPosition(x, y) && nodes[y][x].walkable;
}

Node* Grid::GetNode(int x, int y) {
    if (IsValidPosition(x, y)) {
        return &nodes[y][x];
    }
    return nullptr;
}

void Grid::ResetPathfindingData() {
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            nodes[y][x].gCost = 0;
            nodes[y][x].hCost = 0;
            nodes[y][x].parent = nullptr;
        }
    }
}