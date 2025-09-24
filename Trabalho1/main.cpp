#include "raylib/build/raylib/include/raylib.h"
#include <vector>
#include <iostream>
#include <cmath>
#include <algorithm>
#include <chrono>
#include <fstream>
#include <string>
#include <iomanip>

const float MOVEMENT_THRESHOLD = 2.0f;

enum Shape {
    Circle,
    RectangleShape,
    TriangleShape,
    Line
};

struct MouseMovement {
    double timestamp;
    int x, y;
};

struct ClickEvent {
    double timestamp;
    int x, y;
    int shapeIndex;
    std::string shapeType;
};

struct ShapeInfo {
    Shape shape;
    Color color;
    int x1 = 400;
    int y1 = 400;
    int x2 = 800;
    int y2 = 800;
    int width = 0;
    int height = 0;
    float radius = 0;
    Vector2 v1 = {0,0};
    Vector2 v2 = {0,0};
    Vector2 v3 = {0,0};
    std::string name;

    std::string getTypeString() {
        switch(shape) {
            case Circle: return "Circle";
            case RectangleShape: return "Rectangle";
            case TriangleShape: return "Triangle";
            case Line: return "Line";
            default: return "Unknown";
        }
    }
};

struct SessionInfo {
    std::chrono::steady_clock::time_point startTime;
    std::vector<ClickEvent> clicks;
    std::vector<MouseMovement> movements;
    int totalClicks = 0;
};

SessionInfo currentSession;
bool isLogging = true;

void startSessionLog() {
    currentSession.startTime = std::chrono::steady_clock::now();
    currentSession.clicks.clear();
    currentSession.movements.clear();
    currentSession.totalClicks = 0;
    std::cout << "Sessão iniciada. Logging ativado." << std::endl;
}

void logMouseMovement(int x, int y) {
    if (!isLogging) return;
    
    static Vector2 lastMousePos = {(float)x, (float)y};
    static bool isFirstMovement = true;
    
    if (isFirstMovement) {
        lastMousePos.x = x;
        lastMousePos.y = y;
        isFirstMovement = false;
        return;
    }
    
    float distance = sqrt(pow(x - lastMousePos.x, 2) + pow(y - lastMousePos.y, 2));
    
    if (distance >= MOVEMENT_THRESHOLD) {
        auto now = std::chrono::steady_clock::now();
        auto duration = std::chrono::duration<double>(now - currentSession.startTime);
        
        MouseMovement movement;
        movement.timestamp = duration.count();
        movement.x = x;
        movement.y = y;
        
        currentSession.movements.push_back(movement);
        lastMousePos.x = x;
        lastMousePos.y = y;
    }
}

void logClick(int x, int y, int shapeIndex, const std::string& shapeType) {
    if (!isLogging) return;
    
    auto now = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration<double>(now - currentSession.startTime);
    
    ClickEvent click;
    click.timestamp = duration.count();
    click.x = x;
    click.y = y;
    click.shapeIndex = shapeIndex;
    click.shapeType = shapeType;
    
    currentSession.clicks.push_back(click);
    currentSession.totalClicks++;
    
    std::cout << "Clique registrado: " << click.timestamp << "s, Shape: " 
              << shapeType << " (Index: " << shapeIndex << ")" << std::endl;
}

void exportSessionData() {
    auto endTime = std::chrono::steady_clock::now();
    auto totalDuration = std::chrono::duration<double>(endTime - currentSession.startTime);
    
    std::ofstream sessionFile("session_summary.csv");
    if (sessionFile.is_open()) {
        sessionFile << "TempoTotalSegundos,TotalCliques,TotalMovimentos\n";
        sessionFile << totalDuration.count() << "," 
                   << currentSession.totalClicks << "," 
                   << currentSession.movements.size() << "\n";
        sessionFile.close();
        std::cout << "Resumo da sessão exportado para session_summary.csv" << std::endl;
    }
    
    std::ofstream movementsFile("mouse_movements.csv");
    if (movementsFile.is_open()) {
        movementsFile << "Timestamp,PosX,PosY\n";
        for (const auto& movement : currentSession.movements) {
            movementsFile << std::fixed << std::setprecision(3) 
                         << movement.timestamp << ","
                         << movement.x << ","
                         << movement.y << "\n";
        }
        movementsFile.close();
        std::cout << "Movimentos do mouse exportados para mouse_movements.csv" << std::endl;
    }
    
    std::ofstream clicksFile("clicks_data.csv");
    if (clicksFile.is_open()) {
        clicksFile << "Timestamp,PosX,PosY,ShapeIndex,ShapeType\n";
        for (const auto& click : currentSession.clicks) {
            clicksFile << std::fixed << std::setprecision(3) 
                      << click.timestamp << ","
                      << click.x << ","
                      << click.y << ","
                      << click.shapeIndex << ","
                      << click.shapeType << "\n";
        }
        clicksFile.close();
        std::cout << "Dados de cliques exportados para clicks_data.csv" << std::endl;
    }
}

bool pointOnSegment(const Vector2 &a, const Vector2 &b, int x, int y) {
    Vector2 p = {(float)x, (float)y};
    float cross = (p.x - a.x) * (b.y - a.y) - (p.y - a.y) * (b.x - a.x);
    if (fabs(cross) > 1e-9) return false;
    bool within_x = (x >= std::min(a.x, b.x) && x <= std::max(a.x, b.x));
    bool within_y = (y >= std::min(a.y, b.y) && y <= std::max(a.y, b.y));
    return within_x && within_y;
}

bool pointInCircle(const Vector2 &center, float radius, int x, int y) {
    float dx = x - center.x;
    float dy = y - center.y;
    return (dx * dx + dy * dy) <= (radius * radius);
}

bool pointInRectangle(int rectX, int rectY, int width, int height, int x, int y) {
    return (x >= rectX && x <= rectX + width && y >= rectY && y <= rectY + height);
}

float triangleArea(const Vector2 &a, const Vector2 &b, const Vector2 &c) {
    return fabs((a.x*(b.y-c.y) + b.x*(c.y-a.y) + c.x*(a.y-b.y)) / 2.0f);
}

bool pointInTriangle(const Vector2 &v1, const Vector2 &v2, const Vector2 &v3, int x, int y) {
    Vector2 p = {(float)x, (float)y};
    float area = triangleArea(v1, v2, v3);
    float area1 = triangleArea(p, v2, v3);
    float area2 = triangleArea(v1, p, v3);
    float area3 = triangleArea(v1, v2, p);
    return fabs(area - (area1 + area2 + area3)) < 1e-9;
}

int rayCasting(std::vector<ShapeInfo> &shapes, int x, int y) {
    for (int i = shapes.size() - 1; i >= 0; i--) {
        const ShapeInfo& shape = shapes[i];
        switch(shape.shape) {
            case Circle:
                if (pointInCircle({(float)shape.x1, (float)shape.y1}, shape.radius, x, y)) return i;
                break;
            case RectangleShape:
                if (pointInRectangle(shape.x1, shape.y1, shape.width, shape.height, x, y)) return i;
                break;
            case TriangleShape:
                if (pointInTriangle(shape.v1, shape.v2, shape.v3, x, y)) return i;
                break;
            case Line:
                if (pointOnSegment({(float)shape.x1, (float)shape.y1}, {(float)shape.x2, (float)shape.y2}, x, y)) return i;
                break;
        }
    }
    return -1;
}

int main() 
{
    InitWindow(800, 800, "Salve");
    SetTargetFPS(60);

    std::vector<ShapeInfo> shapes;
    int selectedShape = -1;
    startSessionLog();

    while(!WindowShouldClose())
    {   
        Vector2 mousePos = GetMousePosition();
        logMouseMovement(mousePos.x, mousePos.y);

        // CORREÇÃO: Usar IsKeyPressed() em vez de IsKeyDown()
        if(IsKeyPressed(KEY_C)) {
            std::cout << "Criando Circulo" << std::endl;
            std::cout << "Digite x, y, raio: ";
            int x, y;
            float radius;
            std::cin >> x >> y >> radius;
            Color color = {static_cast<unsigned char>(GetRandomValue(50, 255)),
                         static_cast<unsigned char>(GetRandomValue(50, 255)),
                         static_cast<unsigned char>(GetRandomValue(50, 255)), 255};
            ShapeInfo shape {Circle, color};
            shape.x1 = x;
            shape.y1 = y;
            shape.radius = radius;
            shapes.push_back(shape);
            
        } else if(IsKeyPressed(KEY_R)) {
            std::cout << "Criando Retangulo" << std::endl;
            std::cout << "Digite x, y, largura, altura: ";
            int x, y, w, h;
            std::cin >> x >> y >> w >> h;
            Color color = {static_cast<unsigned char>(GetRandomValue(50, 255)),
                         static_cast<unsigned char>(GetRandomValue(50, 255)),
                         static_cast<unsigned char>(GetRandomValue(50, 255)), 255};
            ShapeInfo shape {RectangleShape, color};
            shape.x1 = x;
            shape.y1 = y;
            shape.width = w;
            shape.height = h;
            shapes.push_back(shape);
            
        } else if(IsKeyPressed(KEY_T)) {
            std::cout << "Criando Triangulo" << std::endl;
            Vector2 v1, v2, v3;
            std::cout << "Digite x1 y1 x2 y2 x3 y3: ";
            std::cin >> v1.x >> v1.y >> v2.x >> v2.y >> v3.x >> v3.y;
            Color color = {static_cast<unsigned char>(GetRandomValue(50, 255)),
                         static_cast<unsigned char>(GetRandomValue(50, 255)),
                         static_cast<unsigned char>(GetRandomValue(50, 255)), 255};
            ShapeInfo shape {TriangleShape, color};
            shape.v1 = v1;
            shape.v2 = v2;
            shape.v3 = v3;
            shapes.push_back(shape);
            
        } else if(IsKeyPressed(KEY_L)) {
            std::cout << "Criando Linha" << std::endl;
            std::cout << "Digite x1 y1 x2 y2: ";
            int x1, y1, x2, y2;
            std::cin >> x1 >> y1 >> x2 >> y2;
            Color color = {static_cast<unsigned char>(GetRandomValue(50, 255)),
                         static_cast<unsigned char>(GetRandomValue(50, 255)),
                         static_cast<unsigned char>(GetRandomValue(50, 255)), 255};
            ShapeInfo shape {Line, color};
            shape.x1 = x1;
            shape.y1 = y1;
            shape.x2 = x2;
            shape.y2 = y2;
            shapes.push_back(shape);
        }
        
        if(IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            selectedShape = rayCasting(shapes, mousePos.x, mousePos.y);
            
            if (selectedShape != -1) {
                std::string shapeType = shapes[selectedShape].getTypeString();
                logClick(mousePos.x, mousePos.y, selectedShape, shapeType);
                
                std::cout << "Shape selecionado: " << shapeType << " (Index: " << selectedShape << ")" << std::endl;
                
                std::cout << "O que deseja modificar?" << std::endl;
                std::cout << "1 - Posição" << std::endl;
                std::cout << "2 - Propriedades específicas" << std::endl;
                std::cout << "3 - Cor" << std::endl;
                std::cout << "0 - Cancelar" << std::endl;
                
                int opcao;
                std::cin >> opcao;
                
                if (opcao == 1) {
                    std::cout << "Digite nova posição (x y): ";
                    int newX, newY;
                    std::cin >> newX >> newY;
                    
                    switch (shapes[selectedShape].shape) {
                        case Circle:
                        case RectangleShape:
                        case Line:
                            shapes[selectedShape].x1 = newX;
                            shapes[selectedShape].y1 = newY;
                            break;
                        case TriangleShape:
                            Vector2 centroAtual = {
                                (shapes[selectedShape].v1.x + shapes[selectedShape].v2.x + shapes[selectedShape].v3.x) / 3,
                                (shapes[selectedShape].v1.y + shapes[selectedShape].v2.y + shapes[selectedShape].v3.y) / 3
                            };
                            Vector2 deslocamento = {(float)(newX - centroAtual.x), (float)(newY - centroAtual.y)};
                            shapes[selectedShape].v1.x += deslocamento.x;
                            shapes[selectedShape].v1.y += deslocamento.y;
                            shapes[selectedShape].v2.x += deslocamento.x;
                            shapes[selectedShape].v2.y += deslocamento.y;
                            shapes[selectedShape].v3.x += deslocamento.x;
                            shapes[selectedShape].v3.y += deslocamento.y;
                            break;
                    }
                    std::cout << "Posição atualizada!" << std::endl;
                    
                } else if (opcao == 2) {
                    switch (shapes[selectedShape].shape) {
                        case Circle:
                            std::cout << "Digite novo raio: ";
                            std::cin >> shapes[selectedShape].radius;
                            break;
                        case RectangleShape:
                            std::cout << "Digite nova largura e altura: ";
                            std::cin >> shapes[selectedShape].width >> shapes[selectedShape].height;
                            break;
                        case TriangleShape:
                            std::cout << "Digite novos vértices (v1.x v1.y v2.x v2.y v3.x v3.y): ";
                            std::cin >> shapes[selectedShape].v1.x >> shapes[selectedShape].v1.y 
                                    >> shapes[selectedShape].v2.x >> shapes[selectedShape].v2.y 
                                    >> shapes[selectedShape].v3.x >> shapes[selectedShape].v3.y;
                            break;
                        case Line:
                            std::cout << "Digite novas coordenadas finais (x2 y2): ";
                            std::cin >> shapes[selectedShape].x2 >> shapes[selectedShape].y2;
                            break;
                    }
                } else if (opcao == 3) {
                    std::cout << "Digite nova cor (R G B): ";
                    int r, g, b;
                    std::cin >> r >> g >> b;
                    shapes[selectedShape].color.r = (unsigned char)r;
                    shapes[selectedShape].color.g = (unsigned char)g;
                    shapes[selectedShape].color.b = (unsigned char)b;
                }
            } else {
                // CORREÇÃO: Este else deve estar aqui, não dentro do if
                logClick(mousePos.x, mousePos.y, -1, "None");
                std::cout << "Nenhum shape selecionado." << std::endl;
            }
        }

        BeginDrawing();
        ClearBackground(BLACK);
        
        for(size_t i = 0; i < shapes.size(); i++) {
            ShapeInfo shape = shapes[i];
            switch(shape.shape) {
                case Circle:
                    DrawCircle(shape.x1, shape.y1, shape.radius, shape.color);
                    break;
                case RectangleShape:
                    DrawRectangle(shape.x1, shape.y1, shape.width, shape.height, shape.color);
                    break;
                case TriangleShape:
                    DrawTriangle(shape.v1, shape.v2, shape.v3, shape.color);
                    break;
                case Line:
                    DrawLine(shape.x1, shape.y1, shape.x2, shape.y2, shape.color);
                    break;
            }
        }

        DrawText(TextFormat("Shapes: %d | Cliques: %d | Movimentos: %d", 
                           shapes.size(), currentSession.totalClicks, currentSession.movements.size()), 
                 10, GetScreenHeight() - 30, 20, GREEN);

        EndDrawing();
    }

    exportSessionData();
    CloseWindow();
    return 0;
}