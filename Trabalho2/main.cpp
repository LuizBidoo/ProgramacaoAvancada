#include "raylib.h"
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

struct Aresta { 
    Vector2 a, b;
    bool operator==(const Aresta& other) const {
        return (a.x == other.a.x && a.y == other.a.y && b.x == other.b.x && b.y == other.b.y) ||
               (a.x == other.b.x && a.y == other.b.y && b.x == other.a.x && b.y == other.a.y);
    }
};

struct Triangulo {
    Vector2 a, b, c;
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
    Vector2 v1 = Vector2();
    Vector2 v2 = Vector2();
    Vector2 v3 = Vector2();
    std::string name;

    std::string getTypeString() const {
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
    float distance = sqrtf((x - lastMousePos.x)*(x - lastMousePos.x) + (y - lastMousePos.y)*(y - lastMousePos.y));
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
    if (fabs(cross) > 1e-9f) return false;
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
    return fabsf((a.x*(b.y-c.y) + b.x*(c.y-a.y) + c.x*(a.y-b.y)) / 2.0f);
}
bool pointInTriangle(const Vector2 &v1, const Vector2 &v2, const Vector2 &v3, int x, int y) {
    Vector2 p = {(float)x, (float)y};
    float area = triangleArea(v1, v2, v3);
    float area1 = triangleArea(p, v2, v3);
    float area2 = triangleArea(v1, p, v3);
    float area3 = triangleArea(v1, v2, p);
    return fabsf(area - (area1 + area2 + area3)) < 1e-3f;
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

Vector2 Circuncentro(const Triangulo& t) {
    float x1 = t.a.x, y1 = t.a.y;
    float x2 = t.b.x, y2 = t.b.y;
    float x3 = t.c.x, y3 = t.c.y;

    float D = 2 * (x1*(y2 - y3) + x2*(y3 - y1) + x3*(y1 - y2));
    if (fabsf(D) < 1e-6f) return { NAN, NAN }; 

    float ux = ((x1*x1 + y1*y1)*(y2 - y3) +
                (x2*x2 + y2*y2)*(y3 - y1) +
                (x3*x3 + y3*y3)*(y1 - y2)) / D;

    float uy = ((x1*x1 + y1*y1)*(x3 - x2) +
                (x2*x2 + y2*y2)*(x1 - x3) +
                (x3*x3 + y3*y3)*(x2 - x1)) / D;

    return {ux, uy};
}

bool PontoDentroCircuncirculo(const Vector2& p, const Triangulo& t) {
    float ax = t.a.x - p.x;
    float ay = t.a.y - p.y;
    float bx = t.b.x - p.x;
    float by = t.b.y - p.y;
    float cx = t.c.x - p.x;
    float cy = t.c.y - p.y;

    float det = (ax * ax + ay * ay) * (bx * cy - cx * by) -
                (bx * bx + by * by) * (ax * cy - cx * ay) +
                (cx * cx + cy * cy) * (ax * by - bx * ay);

    return det > 0.0f;
}

std::vector<Triangulo> Delaunay(const std::vector<Vector2>& pontos) {
    std::vector<Triangulo> triangulos;

    float minX = 0, minY = 0, maxX = 800, maxY = 800;
    Vector2 p1 = { minX - 1000, minY - 1000 };
    Vector2 p2 = { maxX + 1000, minY - 1000 };
    Vector2 p3 = { (minX + maxX) / 2, maxY + 1000 };
    triangulos.push_back({ p1, p2, p3 });

    for (auto& p : pontos) {
        std::vector<Aresta> borda;
        std::vector<Triangulo> novos;

        for (auto& t : triangulos) {
            if (PontoDentroCircuncirculo(p, t)) {
                borda.push_back({ t.a, t.b });
                borda.push_back({ t.b, t.c });
                borda.push_back({ t.c, t.a });
            } else {
                novos.push_back(t);
            }
        }

        std::vector<Aresta> bordaUnica;
        for (auto& e : borda) {
            if (std::count(borda.begin(), borda.end(), e) == 1)
                bordaUnica.push_back(e);
        }

        for (auto& e : bordaUnica) {
            novos.push_back({ e.a, e.b, p });
        }

        triangulos = novos;
    }

    triangulos.erase(std::remove_if(triangulos.begin(), triangulos.end(),
        [&](const Triangulo& t) {
            return (t.a.x < minX - 500 || t.b.x < minX - 500 || t.c.x < minX - 500 ||
                    t.a.y < minY - 500 || t.b.y < minY - 500 || t.c.y < minY - 500 ||
                    t.a.x > maxX + 500 || t.b.x > maxX + 500 || t.c.x > maxX + 500 ||
                    t.a.y > maxY + 500 || t.b.y > maxY + 500 || t.c.y > maxY + 500);
        }),
        triangulos.end());

    return triangulos;
}

std::vector<std::vector<Vector2>> BuildVoronoiCells(const std::vector<Triangulo>& triangulos, const std::vector<Vector2>& pontos) {
    std::vector<std::vector<Vector2>> cells(pontos.size());

    std::vector<Vector2> circuncentros;
    circuncentros.reserve(triangulos.size());
    for (const auto &t : triangulos) circuncentros.push_back(Circuncentro(t));

    for (size_t ti = 0; ti < triangulos.size(); ++ti) {
        const Triangulo &t = triangulos[ti];
        Vector2 cc = circuncentros[ti];
        if (!std::isfinite(cc.x) || !std::isfinite(cc.y)) continue; // pular degenerados

        for (int v = 0; v < 3; ++v) {
            Vector2 vert = (v==0 ? t.a : (v==1 ? t.b : t.c));
            for (size_t pi = 0; pi < pontos.size(); ++pi) {
                if (fabsf(vert.x - pontos[pi].x) < 1e-3f && fabsf(vert.y - pontos[pi].y) < 1e-3f) {
                    cells[pi].push_back(cc);
                    break;
                }
            }
        }
    }

    for (size_t i = 0; i < pontos.size(); ++i) {
        auto &poly = cells[i];
        std::sort(poly.begin(), poly.end(), [](const Vector2 &a, const Vector2 &b){
            if (a.x != b.x) return a.x < b.x;
            return a.y < b.y;
        });
        poly.erase(std::unique(poly.begin(), poly.end(), [](const Vector2 &a, const Vector2 &b){
            return (fabsf(a.x-b.x) < 1e-4f && fabsf(a.y-b.y) < 1e-4f);
        }), poly.end());

        if (poly.size() < 3) continue; 

        Vector2 center = pontos[i];
        std::sort(poly.begin(), poly.end(), [&](const Vector2 &A, const Vector2 &B){
            float angA = atan2f(A.y - center.y, A.x - center.x);
            float angB = atan2f(B.y - center.y, B.x - center.x);
            return angA < angB;
        });
    }

    return cells;
}

int main() 
{
    InitWindow(800, 800, "Salve");
    SetTargetFPS(60);

    std::vector<ShapeInfo> shapes;
    int selectedShape = -1;
    bool showVoronoi = false;
    bool showDelauney = false;

    std::vector<Vector2> pontos;
    std::vector<Triangulo> triangulos;
    std::vector<Color> coresTriangulos;

    std::vector<Color> coresPontos;
    std::vector<std::vector<Vector2>> voronoiCells;

    std::ofstream out("custo_computacional.csv", std::ios::trunc);
    out << "num_pontos,tempo_ms\n";
    out.close();

    startSessionLog();

    while(!WindowShouldClose())
    {   
        Vector2 mousePos = GetMousePosition();
        logMouseMovement((int)mousePos.x, (int)mousePos.y);

        if (IsMouseButtonPressed(MOUSE_RIGHT_BUTTON)) 
        {
            Vector2 p = GetMousePosition();
            pontos.push_back(p);

            coresPontos.push_back({
                (unsigned char)GetRandomValue(40, 255),
                (unsigned char)GetRandomValue(40, 255),
                (unsigned char)GetRandomValue(40, 255),
                255
            });

            if (pontos.size() >= 3) 
            {
                auto start = std::chrono::high_resolution_clock::now();

                triangulos = Delaunay(pontos);

                coresTriangulos.clear();
                for (size_t i = 0; i < triangulos.size(); i++) {
                    coresTriangulos.push_back({
                        (unsigned char)GetRandomValue(100, 255),
                        (unsigned char)GetRandomValue(100, 255),
                        (unsigned char)GetRandomValue(100, 255),
                        200
                    });
                }

                voronoiCells = BuildVoronoiCells(triangulos, pontos);

                auto end = std::chrono::high_resolution_clock::now();
                double tempo = std::chrono::duration<double, std::milli>(end - start).count();

                std::cout << "Triângulos gerados: " << triangulos.size()
                        << " | Pontos: " << pontos.size()
                        << " | Tempo (ms): " << tempo
                        << " | Células Voronoi: " << voronoiCells.size()
                        << std::endl;

                std::ofstream out("custo_computacional.csv", std::ios::app);
                if (out.is_open()) {
                    out << pontos.size() << "," << tempo << "\n";
                    out.close();
                }
            }
        }

        if(IsKeyPressed(KEY_V)) showVoronoi = !showVoronoi;
        if(IsKeyPressed(KEY_M)) showDelauney = !showDelauney;

        if(IsKeyPressed(KEY_C)) {
            std::cout << "Criando Circulo" << std::endl;
            std::cout << "Digite x, y, raio: ";
            int x, y; float radius;
            std::cin >> x >> y >> radius;
            Color color = { (unsigned char)GetRandomValue(50, 255),
                            (unsigned char)GetRandomValue(50, 255),
                            (unsigned char)GetRandomValue(50, 255), 255 };
            ShapeInfo shape { Circle, color };
            shape.x1 = x; shape.y1 = y; shape.radius = radius;
            shapes.push_back(shape);
        }
        else if(IsKeyPressed(KEY_R)) {
            std::cout << "Criando Retangulo" << std::endl;
            std::cout << "Digite x, y, largura, altura: ";
            int x, y, w, h;
            std::cin >> x >> y >> w >> h;
            Color color = { (unsigned char)GetRandomValue(50, 255),
                            (unsigned char)GetRandomValue(50, 255),
                            (unsigned char)GetRandomValue(50, 255), 255 };
            ShapeInfo shape { RectangleShape, color };
            shape.x1 = x; shape.y1 = y; shape.width = w; shape.height = h;
            shapes.push_back(shape);
        }
        else if(IsKeyPressed(KEY_T)) {
            std::cout << "Criando Triangulo" << std::endl;
            Vector2 v1, v2, v3;
            std::cout << "Digite x1 y1 x2 y2 x3 y3: ";
            std::cin >> v1.x >> v1.y >> v2.x >> v2.y >> v3.x >> v3.y;
            Color color = { (unsigned char)GetRandomValue(50, 255),
                            (unsigned char)GetRandomValue(50, 255),
                            (unsigned char)GetRandomValue(50, 255), 255 };
            ShapeInfo shape { TriangleShape, color };
            shape.v1 = v1; shape.v2 = v2; shape.v3 = v3;
            shapes.push_back(shape);
        }
        else if(IsKeyPressed(KEY_L)) {
            std::cout << "Criando Linha" << std::endl;
            std::cout << "Digite x1 y1 x2 y2: ";
            int x1, y1, x2, y2;
            std::cin >> x1 >> y1 >> x2 >> y2;
            Color color = { (unsigned char)GetRandomValue(50, 255),
                            (unsigned char)GetRandomValue(50, 255),
                            (unsigned char)GetRandomValue(50, 255), 255 };
            ShapeInfo shape { Line, color };
            shape.x1 = x1; shape.y1 = y1; shape.x2 = x2; shape.y2 = y2;
            shapes.push_back(shape);
        }

        if(IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            selectedShape = rayCasting(shapes, (int)mousePos.x, (int)mousePos.y);
            if (selectedShape != -1) {
                std::string shapeType = shapes[selectedShape].getTypeString();
                logClick((int)mousePos.x, (int)mousePos.y, selectedShape, shapeType);
                std::cout << "Shape selecionado: " << shapeType << " (Index: " << selectedShape << ")" << std::endl;
                std::cout << "1 - Posição\n2 - Propriedades específicas\n3 - Cor\n0 - Cancelar\n";
                int opcao; std::cin >> opcao;
                if (opcao == 1) {
                    std::cout << "Digite nova posição (x y): ";
                    int newX, newY; std::cin >> newX >> newY;
                    switch (shapes[selectedShape].shape) {
                        case Circle: case RectangleShape: case Line:
                            shapes[selectedShape].x1 = newX; shapes[selectedShape].y1 = newY;
                            break;
                        case TriangleShape: {
                            Vector2 centroAtual = {
                                (shapes[selectedShape].v1.x + shapes[selectedShape].v2.x + shapes[selectedShape].v3.x) / 3,
                                (shapes[selectedShape].v1.y + shapes[selectedShape].v2.y + shapes[selectedShape].v3.y) / 3
                            };
                            Vector2 deslocamento = {(float)(newX - centroAtual.x), (float)(newY - centroAtual.y)};
                            shapes[selectedShape].v1.x += deslocamento.x; shapes[selectedShape].v1.y += deslocamento.y;
                            shapes[selectedShape].v2.x += deslocamento.x; shapes[selectedShape].v2.y += deslocamento.y;
                            shapes[selectedShape].v3.x += deslocamento.x; shapes[selectedShape].v3.y += deslocamento.y;
                            break;
                        }
                    }
                } else if (opcao == 2) {
                    switch (shapes[selectedShape].shape) {
                        case Circle: std::cin >> shapes[selectedShape].radius; break;
                        case RectangleShape: std::cin >> shapes[selectedShape].width >> shapes[selectedShape].height; break;
                        case TriangleShape:
                            std::cin >> shapes[selectedShape].v1.x >> shapes[selectedShape].v1.y 
                                     >> shapes[selectedShape].v2.x >> shapes[selectedShape].v2.y 
                                     >> shapes[selectedShape].v3.x >> shapes[selectedShape].v3.y;
                            break;
                        case Line: std::cin >> shapes[selectedShape].x2 >> shapes[selectedShape].y2; break;
                    }
                } else if (opcao == 3) {
                    int r,g,b; std::cin >> r >> g >> b;
                    shapes[selectedShape].color = {(unsigned char)r,(unsigned char)g,(unsigned char)b,255};
                }
            } else {
                logClick((int)mousePos.x, (int)mousePos.y, -1, "None");
                std::cout << "Nenhum shape selecionado." << std::endl;
            }
        }

        BeginDrawing();
        ClearBackground(WHITE);

        for (const auto& shape : shapes) {
            switch(shape.shape) {
                case Circle: DrawCircle(shape.x1, shape.y1, shape.radius, shape.color); break;
                case RectangleShape: DrawRectangle(shape.x1, shape.y1, shape.width, shape.height, shape.color); break;
                case TriangleShape: DrawTriangle(shape.v1, shape.v2, shape.v3, shape.color); break;
                case Line: DrawLine(shape.x1, shape.y1, shape.x2, shape.y2, shape.color); break;
            }
        }

        for (auto &p : pontos) DrawCircleV(p, 3, RED);

        if (!triangulos.empty() && showDelauney) {
            for (size_t i = 0; i < triangulos.size() && i < coresTriangulos.size(); i++) {
                DrawTriangle(triangulos[i].a, triangulos[i].b, triangulos[i].c, coresTriangulos[i]);
                DrawTriangleLines(triangulos[i].a, triangulos[i].b, triangulos[i].c, RED);
            }
        }

        if (showVoronoi && !voronoiCells.empty()) {
            for (size_t i = 0; i < pontos.size() && i < voronoiCells.size() && i < coresPontos.size(); ++i) {
                auto &poly = voronoiCells[i];
                if (poly.size() < 3) continue; 
                DrawTriangleFan(poly.data(), (int)poly.size(), coresPontos[i]);
                for (size_t k = 0; k < poly.size(); ++k) {
                    Vector2 A = poly[k];
                    Vector2 B = poly[(k+1) % poly.size()];
                    DrawLineV(A, B, BLUE);
                }
                DrawCircleV(pontos[i], 3, BLUE);
            }
        }

        DrawText(TextFormat("Shapes: %d | Cliques: %d | Movimentos: %d", 
                           (int)shapes.size(), currentSession.totalClicks, (int)currentSession.movements.size()), 
                 10, GetScreenHeight() - 30, 20, GREEN);

        EndDrawing();
    }

    exportSessionData();
    CloseWindow();
    return 0;
}
