#include "raylib.h"
#include "Grid.h"
#include "AgentManager.h"
#include "Metrics.h"
#include "factories/NavigationFactory.h"
#include "factories/BasicGridFactory.h"
#include "factories/AStarPathfinderFactory.h"
#include "factories/BasicAgentFactory.h"
#include "factories/RandomObstacleFactory.h"
#include <memory>

void RunPerformanceTests(std::unique_ptr<NavigationFactory>& factory) {
    printf("Iniciando testes de performance...\n");
    
    std::vector<std::pair<int, int>> gridSizes = {{10, 10}, {20, 20}, {40, 40}};
    std::vector<int> agentCounts = {1, 5, 10, 20};
    
    for (auto& gridSize : gridSizes) {
        int width = gridSize.first;
        int height = gridSize.second;
        float cellSize = 20.0f;
        
        auto grid = factory->CreateGrid(width, height, cellSize);
        auto agentManager = factory->CreateAgentManager(grid.get());
        
        factory->CreateObstacles(*grid, (width * height) / 8);
        
        for (int agents : agentCounts) {
            printf("Testando: Grid %dx%d com %d agentes\n", width, height, agents);
            
            for (int i = 0; i < agents; i++) {
                Vector2 start, target;
                
                do {
                    start = {(float)GetRandomValue(0, width-1), 
                            (float)GetRandomValue(0, height-1)};
                } while (!grid->IsWalkable((int)start.x, (int)start.y));
                
                do {
                    target = {(float)GetRandomValue(0, width-1), 
                             (float)GetRandomValue(0, height-1)};
                } while (!grid->IsWalkable((int)target.x, (int)target.y) || 
                        (start.x == target.x && start.y == target.y));
                
                agentManager->AddAgent(start, target);
            }
            
            for (int frame = 0; frame < 60; frame++) {
                agentManager->UpdateAll(1.0f/60.0f);
            }
        }
    }
    
    Metrics::SaveToCSV("performance_data.csv");
    printf("Testes concluídos! Dados salvos em performance_data.csv\n");
}

int main() {
    const int screenWidth = 800;
    const int screenHeight = 600;
    const float cellSize = 60.0f;
    const int numCols = screenWidth / cellSize;
    const int numRows = screenHeight / cellSize;

    // Configuração das fábricas
    auto navigationFactory = std::make_unique<NavigationFactory>(
        std::make_unique<BasicGridFactory>(),
        std::make_unique<AStarPathfinderFactory>(),
        std::make_unique<BasicAgentFactory>(),
        std::make_unique<RandomObstacleFactory>()
    );

    // Criação dos componentes usando as fábricas
    auto grid = navigationFactory->CreateGrid(numCols, numRows, cellSize);
    auto agentManager = navigationFactory->CreateAgentManager(grid.get());

    Vector2 spawnPos = {-1, -1};
    Vector2 targetPos = {-1, -1};
    bool placingSpawn = false;
    bool placingTarget = false;

    InitWindow(screenWidth, screenHeight, "Grid Navigation with Factories");

    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        Vector2 mousePos = GetMousePosition();
        int gridX = mousePos.x / cellSize;
        int gridY = mousePos.y / cellSize;

        if (IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
            navigationFactory->CreateObstacleAt(*grid, gridX, gridY);
        }
        
        if (IsMouseButtonPressed(MOUSE_RIGHT_BUTTON)) {
            if (placingSpawn && grid->IsWalkable(gridX, gridY)) {
                spawnPos = {(float)gridX, (float)gridY};
                placingSpawn = false;
                placingTarget = false;
            }
            else if (placingTarget && grid->IsWalkable(gridX, gridY)) {
                targetPos = {(float)gridX, (float)gridY};
                placingTarget = false;
                placingSpawn = false;
            }
            else {
                grid->SetOccupied(gridX, gridY, false);
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
                grid->IsWalkable((int)spawnPos.x, (int)spawnPos.y) && 
                grid->IsWalkable((int)targetPos.x, (int)targetPos.y)) {
                agentManager->AddAgent(spawnPos, targetPos);
                spawnPos = {-1, -1};
                targetPos = {-1, -1};
            }
        }

        if (IsKeyPressed(KEY_R)) {
            agentManager->AddRandomAgents(5);
        }

        if (IsKeyPressed(KEY_P)) {
            Metrics::Clear();
            RunPerformanceTests(navigationFactory);
        }

        if (IsKeyPressed(KEY_M)) {
            Metrics::SaveToCSV("manual_performance_data.csv");
            printf("Métricas salvas manualmente em manual_performance_data.csv!\n");
        }

        agentManager->UpdateAll(GetFrameTime());

        BeginDrawing();
            ClearBackground(RAYWHITE);
            
            grid->Draw();
            agentManager->DrawAll(*grid);
                       
            
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
            DrawText(TextFormat("Agents: %d", agentManager->GetAgentCount()), 10, 160, 20, DARKGRAY);
            
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