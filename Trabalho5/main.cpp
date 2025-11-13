#include "raylib.h"
#include <vector>
#include <algorithm>
#include <cmath>
#include <functional>
#include <queue>
#include <memory>
#include <fstream>
#include <chrono>

struct MetricData {
    int agentCount;
    int gridWidth;
    int gridHeight;
    double pathfindingTime;
    int pathLength;
    std::string distributionType;
};

class Metrics {
private:
    static std::vector<MetricData> data;
    
public:
    static void RecordPathfinding(int agents, int gridW, int gridH, 
                                double time, int pathLen, const std::string& dist) {
        data.push_back({agents, gridW, gridH, time, pathLen, dist});
    }
    
    static void SaveToCSV(const std::string& filename) {
        std::ofstream file(filename);
        file << "agents,grid_width,grid_height,time_ms,path_length,distribution\n";
        
        for (const auto& metric : data) {
            file << metric.agentCount << ","
                 << metric.gridWidth << ","
                 << metric.gridHeight << ","
                 << metric.pathfindingTime * 1000 << ","
                 << metric.pathLength << ","
                 << metric.distributionType << "\n";
        }
        file.close();
    }
    
    static void Clear() { data.clear(); }

};

std::vector<MetricData> Metrics::data;

// Classes necessárias para o algoritmo de navegação A*

class Node {
public:
    int x, y;
    bool occupied;
    bool walkable;
    
    float gCost;
    float hCost;
    float fCost() const { return gCost + hCost; }
    
    Node* parent;
    
    Node() : x(0), y(0), occupied(false), walkable(true), 
             gCost(0), hCost(0), parent(nullptr) {}
    
    Node(int x, int y) : x(x), y(y), occupied(false), walkable(true), 
                         gCost(0), hCost(0), parent(nullptr) {}
    
    bool operator==(const Node& other) const {
        return x == other.x && y == other.y;
    }
};

class Grid {
private:
    int width, height;
    float cell_size;
    std::vector<std::vector<Node>> nodes;
    
public:
    Grid(int w, int h, float cell_size) : width(w), height(h), cell_size(cell_size) {
        nodes.resize(height);
        for (int y = 0; y < height; y++) {
            nodes[y].resize(width);
            for (int x = 0; x < width; x++) {
                nodes[y][x] = Node(x, y); 
            }
        }
    }
    
    void Draw() {
        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                Color color = nodes[y][x].occupied ? RED : GREEN;
                if (!nodes[y][x].walkable) color = DARKGRAY;
                
                DrawRectangle(x * cell_size, y * cell_size, cell_size - 1, cell_size - 1, color);
                DrawRectangleLines(x * cell_size, y * cell_size, cell_size, cell_size, LIGHTGRAY);
            }
        }
    }
    
    void SetOccupied(int x, int y, bool occupied) {
        if (IsValidPosition(x, y)) {
            nodes[y][x].occupied = occupied;
            nodes[y][x].walkable = !occupied;
        }
    }
    
    void SetWalkable(int x, int y, bool walkable) {
        if (IsValidPosition(x, y)) {
            nodes[y][x].walkable = walkable;
            nodes[y][x].occupied = !walkable;
        }
    }
    
    bool IsValidPosition(int x, int y) const {
        return x >= 0 && x < width && y >= 0 && y < height;
    }
    
    bool IsWalkable(int x, int y) const {
        return IsValidPosition(x, y) && nodes[y][x].walkable;
    }
    
    Node* GetNode(int x, int y) {
        if (IsValidPosition(x, y)) {
            return &nodes[y][x];
        }
        return nullptr;
    }
    
    void ResetPathfindingData() {
        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                nodes[y][x].gCost = 0;
                nodes[y][x].hCost = 0;
                nodes[y][x].parent = nullptr;
            }
        }
    }
    
    int GetWidth() const { return width; }
    int GetHeight() const { return height; }
    float GetCellSize() const { return cell_size; }
};

class Pathfinder {
private:
    static double lastExecutionTime;
    
    static float CalculateHeuristic(int x1, int y1, int x2, int y2) {
        return abs(x1 - x2) + abs(y1 - y2);
    }
    
    static std::vector<Node*> GetNeighbors(Node* node, Grid& grid) {
        std::vector<Node*> neighbors;
        int directions[4][2] = {{0, 1}, {1, 0}, {0, -1}, {-1, 0}};
        
        for (auto& dir : directions) {
            int newX = node->x + dir[0];
            int newY = node->y + dir[1];
            
            if (grid.IsWalkable(newX, newY)) {
                Node* neighbor = grid.GetNode(newX, newY);
                if (neighbor) {
                    neighbors.push_back(neighbor);
                }
            }
        }
        return neighbors;
    }
    
    static std::vector<Vector2> ReconstructPath(Node* endNode) {
        std::vector<Vector2> path;
        Node* currentNode = endNode;
        
        while (currentNode != nullptr) {
            path.push_back({(float)currentNode->x, (float)currentNode->y});
            currentNode = currentNode->parent;
        }
        
        std::reverse(path.begin(), path.end());
        return path;
    }

public:
    static std::vector<Vector2> FindPath(Grid& grid, Vector2 start, Vector2 end, const std::string& distribution = "random") {
        double startTime = GetTime();
        
        grid.ResetPathfindingData();
        
        Node* startNode = grid.GetNode((int)start.x, (int)start.y);
        Node* endNode = grid.GetNode((int)end.x, (int)end.y);
        
        if (!startNode || !endNode || !startNode->walkable || !endNode->walkable) {
            return {};
        }
        
        std::vector<Node*> openSet;
        std::vector<Node*> closedSet;
        
        openSet.push_back(startNode);
        
        while (!openSet.empty()) {
            Node* currentNode = openSet[0];
            int currentIndex = 0;
            
            for (int i = 1; i < openSet.size(); i++) {
                if (openSet[i]->fCost() < currentNode->fCost() || 
                    (openSet[i]->fCost() == currentNode->fCost() && openSet[i]->hCost < currentNode->hCost)) {
                    currentNode = openSet[i];
                    currentIndex = i;
                }
            }
            
            openSet.erase(openSet.begin() + currentIndex);
            closedSet.push_back(currentNode);
            
            if (currentNode == endNode) {
                lastExecutionTime = GetTime() - startTime;
                std::vector<Vector2> finalPath = ReconstructPath(currentNode);
                
                Metrics::RecordPathfinding(1, grid.GetWidth(), grid.GetHeight(), 
                                         lastExecutionTime, finalPath.size(), distribution);
                
                return finalPath;
            }
            
            auto neighbors = GetNeighbors(currentNode, grid);
            for (auto neighbor : neighbors) {
                if (std::find(closedSet.begin(), closedSet.end(), neighbor) != closedSet.end()) {
                    continue;
                }
                
                float newGCost = currentNode->gCost + 1;
                
                if (newGCost < neighbor->gCost || 
                    std::find(openSet.begin(), openSet.end(), neighbor) == openSet.end()) {
                    
                    neighbor->gCost = newGCost;
                    neighbor->hCost = CalculateHeuristic(neighbor->x, neighbor->y, endNode->x, endNode->y);
                    neighbor->parent = currentNode;
                    
                    if (std::find(openSet.begin(), openSet.end(), neighbor) == openSet.end()) {
                        openSet.push_back(neighbor);
                    }
                }
            }
        }
        
        lastExecutionTime = GetTime() - startTime;
        
        Metrics::RecordPathfinding(1, grid.GetWidth(), grid.GetHeight(), 
                                 lastExecutionTime, 0, distribution);
        
        return {};
    }
    
    static double GetLastExecutionTime() {
        return lastExecutionTime;
    }
};

double Pathfinder::lastExecutionTime = 0.0;

class Agent {
private:
    Vector2 position;
    Vector2 target;
    std::vector<Vector2> path;
    bool has_path;
    Color color;
    float speed;
    int currentPathIndex;
    
public:
    Agent(Vector2 start, Vector2 target) : position(start), target(target), has_path(false), 
                                          color(GetRandomColor()), speed(2.0f), currentPathIndex(0) {}
    
    void Update(Grid& grid, float delta_time) {
        if (!has_path) {
            FindPath(grid);
            return;
        }
        
        if (currentPathIndex < path.size()) {
            Vector2 nextCell = path[currentPathIndex];
            Vector2 targetWorldPos = {nextCell.x * grid.GetCellSize() + grid.GetCellSize() / 2, 
                                     nextCell.y * grid.GetCellSize() + grid.GetCellSize() / 2};
            
            Vector2 direction = {targetWorldPos.x - position.x, targetWorldPos.y - position.y};
            float distance = sqrt(direction.x * direction.x + direction.y * direction.y);
            
            if (distance < 5.0f) {
                currentPathIndex++;
            } else {
                direction.x /= distance;
                direction.y /= distance;
                position.x += direction.x * speed * delta_time * 60.0f;
                position.y += direction.y * speed * delta_time * 60.0f;
            }
        } else {
            has_path = false;
        }
    }
    
    void Draw(Grid& grid) {
        DrawCircle(position.x, position.y, grid.GetCellSize() / 3, color);
    }
    
    void FindPath(Grid& grid) {
        Vector2 gridStart = {position.x / grid.GetCellSize(), position.y / grid.GetCellSize()};
        path = Pathfinder::FindPath(grid, gridStart, target, "random");
        has_path = !path.empty();
        currentPathIndex = 0;
    }
    
    static Color GetRandomColor() {
        Color colors[] = {BLUE, PURPLE, ORANGE, PINK, DARKBLUE, DARKPURPLE};
        return colors[GetRandomValue(0, 5)];
    }
    
    bool HasReachedTarget() const { return !has_path && currentPathIndex >= path.size(); }
};

class AgentManager {
private:
    std::vector<Agent> agents;
    Grid* grid;
    
public:
    AgentManager(Grid* grid) : grid(grid) {}
    
    void AddAgent(Vector2 start, Vector2 target) {
        Vector2 worldStart = {start.x * grid->GetCellSize() + grid->GetCellSize() / 2, 
                             start.y * grid->GetCellSize() + grid->GetCellSize() / 2};
        agents.emplace_back(worldStart, target);
    }
    
    void AddRandomAgents(int count) {
        for (int i = 0; i < count; i++) {
            Vector2 start, target;
            
            do {
                start = {(float)GetRandomValue(0, grid->GetWidth() - 1), 
                        (float)GetRandomValue(0, grid->GetHeight() - 1)};
            } while (!grid->IsWalkable((int)start.x, (int)start.y));
            
            do {
                target = {(float)GetRandomValue(0, grid->GetWidth() - 1), 
                         (float)GetRandomValue(0, grid->GetHeight() - 1)};
            } while (!grid->IsWalkable((int)target.x, (int)target.y) || 
                    (start.x == target.x && start.y == target.y));
            
            AddAgent(start, target);
        }
    }
    
    void UpdateAll(float delta_time) {
        for (auto& agent : agents) {
            agent.Update(*grid, delta_time);
        }
    }
    
    void DrawAll(Grid& grid) {
        for (auto& agent : agents) {
            agent.Draw(grid);
        }
    }
    
    int GetAgentCount() const { return agents.size(); }
};

// Função de testes de performance
void RunPerformanceTests() {
    printf("Iniciando testes de performance...\n");
    
    std::vector<std::pair<int, int>> gridSizes = {{10, 10}, {20, 20}, {40, 40}};
    std::vector<int> agentCounts = {1, 5, 10, 20};
    
    for (auto& gridSize : gridSizes) {
        int width = gridSize.first;
        int height = gridSize.second;
        float cellSize = 20.0f;
        
        Grid grid(width, height, cellSize);
        
        for (int agents : agentCounts) {
            AgentManager manager(&grid);
            
            for (int i = 0; i < (width * height) / 8; i++) {
                int x = GetRandomValue(0, width-1);
                int y = GetRandomValue(0, height-1);
                if (GetRandomValue(0, 100) < 25) { // 25% de chance
                    grid.SetOccupied(x, y, true);
                }
            }
            
            printf("Testando: Grid %dx%d com %d agentes\n", width, height, agents);
            manager.AddRandomAgents(agents);
            
            for (int frame = 0; frame < 60; frame++) {
                manager.UpdateAll(1.0f/60.0f);
            }
        }
    }
    
    Metrics::SaveToCSV("performance_data.csv");
    printf("Testes concluídos! Dados salvos em performance_data.csv\n");
}

// Main

int main()
{
    const int screenWidth = 800;
    const int screenHeight = 600;
    const float cellSize = 60.0f;
    const int numCols = screenWidth / cellSize;
    const int numRows = screenHeight / cellSize;

    Grid grid(numCols, numRows, cellSize);
    AgentManager agentManager(&grid);

    Vector2 spawnPos = {-1, -1};
    Vector2 targetPos = {-1, -1};
    bool placingSpawn = false;
    bool placingTarget = false;

    InitWindow(screenWidth, screenHeight, "Grid Navigation");

    SetTargetFPS(60);

    while (!WindowShouldClose())
    {
        Vector2 mousePos = GetMousePosition();
        int gridX = mousePos.x / cellSize;
        int gridY = mousePos.y / cellSize;

        if (IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
            grid.SetOccupied(gridX, gridY, true);
        }
        
        if (IsMouseButtonPressed(MOUSE_RIGHT_BUTTON)) {
            if (placingSpawn && grid.IsWalkable(gridX, gridY)) {
                spawnPos = {(float)gridX, (float)gridY};
                placingSpawn = false;
                placingTarget = false;
            }
            else if (placingTarget && grid.IsWalkable(gridX, gridY)) {
                targetPos = {(float)gridX, (float)gridY};
                placingTarget = false;
                placingSpawn = false;
            }
            else {
                grid.SetOccupied(gridX, gridY, false);
            }
        }

        if (IsKeyPressed(KEY_S)) {
            placingSpawn = true;
            placingTarget = false;
        }
        if (IsKeyPressed(KEY_T)) {
            placingTarget = true;
            placingSpawn = false;
        }
        
        if (IsKeyPressed(KEY_ESCAPE)) {
            placingSpawn = false;
            placingTarget = false;
        }

        if (IsKeyPressed(KEY_ENTER)) {
            if (spawnPos.x >= 0 && spawnPos.y >= 0 && targetPos.x >= 0 && targetPos.y >= 0 &&
                grid.IsWalkable((int)spawnPos.x, (int)spawnPos.y) && 
                grid.IsWalkable((int)targetPos.x, (int)targetPos.y)) {
                agentManager.AddAgent(spawnPos, targetPos);
                spawnPos = {-1, -1};
                targetPos = {-1, -1};
            }
        }

        if (IsKeyPressed(KEY_R)) {
            agentManager.AddRandomAgents(5);
        }

        if (IsKeyPressed(KEY_P)) {
            Metrics::Clear();
            RunPerformanceTests();
        }

        if (IsKeyPressed(KEY_M)) {
            Metrics::SaveToCSV("manual_performance_data.csv");
            printf("Métricas salvas manualmente em manual_performance_data.csv!\n");
        }

        agentManager.UpdateAll(GetFrameTime());

        BeginDrawing();
            ClearBackground(RAYWHITE);
            
            grid.Draw();
            agentManager.DrawAll(grid);
                       
            
            if (spawnPos.x >= 0 && spawnPos.y >= 0) {
                DrawRectangle(spawnPos.x * cellSize, spawnPos.y * cellSize, cellSize, cellSize, BLUE);
                DrawText("SPAWN", spawnPos.x * cellSize, spawnPos.y * cellSize - 15, 10, BLUE);
            }
            if (targetPos.x >= 0 && targetPos.y >= 0) {
                DrawRectangle(targetPos.x * cellSize, targetPos.y * cellSize, cellSize, cellSize, ORANGE);
                DrawText("TARGET", targetPos.x * cellSize, targetPos.y * cellSize - 15, 10, ORANGE);
            }

            DrawText("Left click: Place obstacle", 10, 10, 20, DARKGRAY);
            DrawText("Right click: Remove obstacle / Place spawn/target", 10, 35, 20, DARKGRAY);
            DrawText("S: Set spawn mode | T: Set target mode", 10, 60, 20, DARKGRAY);
            DrawText("ENTER: Create agent | R: 5 random agents", 10, 85, 20, DARKGRAY);
            DrawText("P: Run performance tests | M: Save metrics", 10, 110, 20, DARKGRAY);
            DrawText("ESC: Cancel placement mode", 10, 135, 20, DARKGRAY);
            DrawText(TextFormat("Agents: %d", agentManager.GetAgentCount()), 10, 160, 20, DARKGRAY);
            
            if (placingSpawn) {
                DrawText("MODE: Placing SPAWN (Right click to place)", 10, 185, 20, BLUE);
            } else if (placingTarget) {
                DrawText("MODE: Placing TARGET (Right click to place)", 10, 185, 20, ORANGE);
            }
            
        EndDrawing();
    }

    CloseWindow();

    return 0;
}