#include "raylib.h"
#include "convexHull.h"
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
    std::vector<Vector2> points;
    float radius = 0;
    float x1 = 0, y1 = 0, x2 = 0, y2 = 0;
    float width = 0, height = 0;
    Vector2 v1 = {0, 0}, v2 = {0, 0}, v3 = {0, 0};

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

float Vector2DistanceCustom(Vector2 v1, Vector2 v2) {
    float dx = v1.x - v2.x;
    float dy = v1.y - v2.y;
    return sqrtf(dx * dx + dy * dy);
}

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
        if (!std::isfinite(cc.x) || !std::isfinite(cc.y)) continue; 

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

void randomPoints(std::vector<Vector2> & pontos, int n) {
    pontos.clear();
    for(size_t i = 0; i < n; i++) {
        pontos.push_back({(float)GetRandomValue(50,750), (float)GetRandomValue(50,750)});
    }
}

int main() 
{
    InitWindow(800, 800, "Salve");
    SetTargetFPS(60);

    std::vector<ShapeInfo> shapes; // desenhos
    // booleanos de verificação
    int selectedShape = -1;
    bool showVoronoi = false;
    bool showDelauney = false;
    bool showEnvoltoria = false;
    bool drawingMode = false;

    std::vector<Vector2> pontos;
    std::vector<Vector2> currentShape;

    std::vector<Triangulo> triangulos;
    std::vector<Color> coresTriangulos;
    std::vector<Color> coresPontos;
    std::vector<std::vector<Vector2>> voronoiCells; 
    std::vector<Vector2> envoltoria;

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

                envoltoria = convexHull(pontos);

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

        if(IsKeyPressed(KEY_P)) {
            int nPoints;
            std::cout << "Quantos pontos aleatórios?" << std::endl;
            std::cin >> nPoints;
            randomPoints(pontos, nPoints);

            coresPontos.clear();
            for (size_t i = 0; i < pontos.size(); i++) {
                coresPontos.push_back({
                    (unsigned char)GetRandomValue(40, 255),
                    (unsigned char)GetRandomValue(40, 255),
                    (unsigned char)GetRandomValue(40, 255),
                    255
                });
            }

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

                envoltoria = convexHull(pontos);

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
        if(IsKeyPressed(KEY_E)) showEnvoltoria = !showEnvoltoria;

        if(IsKeyPressed(KEY_D)) drawingMode = !drawingMode;

        if(drawingMode) {
            if(IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                currentShape.push_back(mousePos);
                
                logClick((int)mousePos.x, (int)mousePos.y, shapes.size(), "DrawingPoint");
            }

            if(IsKeyPressed(KEY_C) && currentShape.size() >= 2) {
                ShapeInfo circle;
                circle.shape = Circle;
                circle.x1 = currentShape[0].x;
                circle.y1 = currentShape[0].y;
                circle.radius = Vector2DistanceCustom(currentShape[0], currentShape[1]);
                circle.color = { 
                    (unsigned char)GetRandomValue(50, 255),
                    (unsigned char)GetRandomValue(50, 255),
                    (unsigned char)GetRandomValue(50, 255),
                    255 
                };
                shapes.push_back(circle);
                currentShape.clear();
                drawingMode = false;
            }
            else if(IsKeyPressed(KEY_R) && currentShape.size() >= 2) {
                ShapeInfo rectangle;
                rectangle.shape = RectangleShape;
                rectangle.x1 = currentShape[0].x;
                rectangle.y1 = currentShape[0].y;
                rectangle.width = currentShape[1].x - currentShape[0].x;
                rectangle.height = currentShape[1].y - currentShape[0].y;
                rectangle.color = { 
                    (unsigned char)GetRandomValue(50, 255),
                    (unsigned char)GetRandomValue(50, 255),
                    (unsigned char)GetRandomValue(50, 255),
                    255 
                };
                shapes.push_back(rectangle);
                currentShape.clear();
                drawingMode = false;
            }
            else if(IsKeyPressed(KEY_T) && currentShape.size() >= 3) {
                // Create triangle from three points
                ShapeInfo triangle;
                triangle.shape = TriangleShape;
                triangle.v1 = currentShape[0];
                triangle.v2 = currentShape[1];
                triangle.v3 = currentShape[2];
                triangle.color = { 
                    (unsigned char)GetRandomValue(50, 255),
                    (unsigned char)GetRandomValue(50, 255),
                    (unsigned char)GetRandomValue(50, 255),
                    255 
                };
                shapes.push_back(triangle);
                currentShape.clear();
                drawingMode = false;
            }
            else if(IsKeyPressed(KEY_L) && currentShape.size() >= 2) {
                // Create line from two points
                ShapeInfo line;
                line.shape = Line;
                line.x1 = currentShape[0].x;
                line.y1 = currentShape[0].y;
                line.x2 = currentShape[1].x;
                line.y2 = currentShape[1].y;
                line.color = { 
                    (unsigned char)GetRandomValue(50, 255),
                    (unsigned char)GetRandomValue(50, 255),
                    (unsigned char)GetRandomValue(50, 255),
                    255 
                };
                shapes.push_back(line);
                currentShape.clear();
                drawingMode = false;
            }
            
            if(IsKeyPressed(KEY_ESCAPE)) {
                currentShape.clear();
                drawingMode = false;
            }
        }

        BeginDrawing();
        ClearBackground(BLACK);

        for (const auto& shape : shapes) {
            switch(shape.shape) {
                case Circle: 
                    DrawCircle((int)shape.x1, (int)shape.y1, shape.radius, shape.color); 
                    break;
                case RectangleShape: 
                    DrawRectangle((int)shape.x1, (int)shape.y1, (int)shape.width, (int)shape.height, shape.color); 
                    break;
                case TriangleShape: 
                    DrawTriangle(shape.v1, shape.v2, shape.v3, shape.color); 
                    break;
                case Line: 
                    DrawLine((int)shape.x1, (int)shape.y1, (int)shape.x2, (int)shape.y2, shape.color); 
                    break;
            }
        }

        for (size_t i = 0; i < pontos.size(); i++) {
            DrawCircleV(pontos[i], 3, coresPontos[i]);
        }

        if (drawingMode && !currentShape.empty()) {
            for (size_t i = 0; i < currentShape.size(); i++) {
                DrawCircleV(currentShape[i], 5, YELLOW);
                if (i > 0) {
                    DrawLineV(currentShape[i-1], currentShape[i], YELLOW);
                }
            }
            if (!currentShape.empty()) {
                DrawLineV(currentShape.back(), mousePos, YELLOW);
            }
        }

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
                
                for (size_t k = 2; k < poly.size(); k++) {
                    DrawTriangle(poly[0], poly[k-1], poly[k], Fade(coresPontos[i], 0.3f));
                }
                
                for (size_t k = 0; k < poly.size(); ++k) {
                    Vector2 A = poly[k];
                    Vector2 B = poly[(k+1) % poly.size()];
                    DrawLineV(A, B, BLUE);
                }
                DrawCircleV(pontos[i], 3, BLUE);
            }
        }

        if (showEnvoltoria && !envoltoria.empty()) {
            for (size_t i = 0; i < envoltoria.size(); i++) {
                size_t next = (i + 1) % envoltoria.size();
                DrawLineV(envoltoria[i], envoltoria[next], RED);
            }
        }

        std::string statusText = "Mostrando: ";

        if (showDelauney) statusText += "[Delaunay] ";
        if (showVoronoi) statusText += "[Voronoi] ";
        if (showEnvoltoria) statusText += "[Envoltória] ";

        if (!showDelauney && !showVoronoi && !showEnvoltoria)
            statusText = "Nenhuma visualização ativa";

        if (drawingMode) {
            statusText += " | Modo Desenho Ativo (C=Circle, R=Rect, T=Triangle, L=Line, ESC=Cancel)";
        }

        DrawText(statusText.c_str(), 20, 20, 20, GREEN);

        EndDrawing();
    }

    exportSessionData();
    CloseWindow();
    return 0;
}