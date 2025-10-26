#include "raylib.h"
#include "convexHull.h"
#include "minkowski.h"
#include <vector>
#include <iostream>
#include <cmath>
#include <algorithm>
#include <chrono>
#include <fstream>
#include <string>
#include <iomanip>
#include <cfloat>

const float MOVEMENT_THRESHOLD = 2.0f;

enum Shape {
    Circle,
    RectangleShape,
    TriangleShape,
    Line,
    Polygon
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
    bool selected = false;

    std::string getTypeString() const {
        switch(shape) {
            case Circle: return "Circle";
            case RectangleShape: return "Rectangle";
            case TriangleShape: return "Triangle";
            case Line: return "Line";
            case Polygon: return "Polygon";
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

void exportMinkowskiToCSV(int robotIndex, int obstacleIndex, const std::vector<Vector2>& minkowskiResult, 
                         const std::vector<Vector2>& robotPoly, const std::vector<Vector2>& obstaclePoly) {
    std::ofstream minkowskiFile("minkowski_results.csv", std::ios::app);
    if (minkowskiFile.is_open()) {
        minkowskiFile.seekp(0, std::ios::end);
        if (minkowskiFile.tellp() == 0) {
            minkowskiFile << "RobotIndex,ObstacleIndex,RobotPoints,ObstaclePoints,MinkowskiPoints,Timestamp\n";
        }
        
        auto now = std::chrono::steady_clock::now();
        auto duration = std::chrono::duration<double>(now - currentSession.startTime);
        
        std::string robotPointsStr;
        for (const auto& p : robotPoly) {
            robotPointsStr += "(" + std::to_string((int)p.x) + "," + std::to_string((int)p.y) + ");";
        }
        if (!robotPointsStr.empty()) robotPointsStr.pop_back();
        
        std::string obstaclePointsStr;
        for (const auto& p : obstaclePoly) {
            obstaclePointsStr += "(" + std::to_string((int)p.x) + "," + std::to_string((int)p.y) + ");";
        }
        if (!obstaclePointsStr.empty()) obstaclePointsStr.pop_back();
        
        std::string minkowskiPointsStr;
        for (const auto& p : minkowskiResult) {
            minkowskiPointsStr += "(" + std::to_string((int)p.x) + "," + std::to_string((int)p.y) + ");";
        }
        if (!minkowskiPointsStr.empty()) minkowskiPointsStr.pop_back();
        
        minkowskiFile << robotIndex << ","
                     << obstacleIndex << ","
                     << "\"" << robotPointsStr << "\","
                     << "\"" << obstaclePointsStr << "\","
                     << "\"" << minkowskiPointsStr << "\","
                     << std::fixed << std::setprecision(3) << duration.count()
                     << "\n";
        
        minkowskiFile.close();
        std::cout << "Resultado Minkowski exportado para minkowski_results.csv" << std::endl;
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
            case Polygon: {
                if (shape.points.size() < 3) return -1;
                bool inside = false;
                for (size_t i = 0, j = shape.points.size() - 1; i < shape.points.size(); j = i++) {
                    if (((shape.points[i].y > y) != (shape.points[j].y > y)) &&
                        (x < (shape.points[j].x - shape.points[i].x) * (y - shape.points[i].y) / 
                             (shape.points[j].y - shape.points[i].y) + shape.points[i].x)) {
                        inside = !inside;
                    }
                }
                if (inside) return i;
                break;
            }
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

Vector2 calculateCentroid(const std::vector<Vector2>& polygon) {
    if (polygon.empty()) {
        return {0, 0};
    }
    Vector2 centroid = {0, 0};
    for (const auto& v : polygon) {
        centroid.x += v.x;
        centroid.y += v.y;
    }
    centroid.x /= polygon.size();
    centroid.y /= polygon.size();
    return centroid;
}

float minDistanceBetweenPolygons(const std::vector<Vector2>& polyA, const std::vector<Vector2>& polyB) {
    float minDist = FLT_MAX;
    for (const auto& a : polyA) {
        for (const auto& b : polyB) {
            float dx = a.x - b.x;
            float dy = a.y - b.y;
            float dist = std::sqrt(dx * dx + dy * dy);
            if (dist < minDist) minDist = dist;
        }
    }
    return minDist;
}

int main() 
{
    InitWindow(800, 800, "Sistema de Soma de Minkowski");
    SetTargetFPS(60);

    std::vector<ShapeInfo> shapes;
    int selectedShape = -1;
    bool showVoronoi = false;
    bool showDelauney = false;
    bool showEnvoltoria = false;
    bool drawingMode = false;

    int robotPolygon = -1;
    std::vector<std::vector<Vector2>> minkowskiResults;
    bool showMinkowski = false;

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

    std::ofstream clearMinkowskiFile("minkowski_results.csv", std::ios::trunc);
    clearMinkowskiFile.close();

    startSessionLog();

    while(!WindowShouldClose())
    {   
        Vector2 mousePos = GetMousePosition();
        logMouseMovement((int)mousePos.x, (int)mousePos.y);

        if(drawingMode) {
            if(IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                currentShape.push_back(mousePos);
            }

            if(IsKeyPressed(KEY_N)) {
                if (currentShape.size() >= 3) {
                    ShapeInfo polygon;
                    polygon.shape = Polygon;
                    polygon.points = convexHull(currentShape);
                    polygon.color = { 
                        (unsigned char)GetRandomValue(50, 255), 
                        (unsigned char)GetRandomValue(50, 255), 
                        (unsigned char)GetRandomValue(50, 255), 
                        255 
                    };
                    shapes.push_back(polygon);
                    currentShape.clear();
                    drawingMode = false;
                    std::cout << "Polígono criado. Total: " << shapes.size() << std::endl;
                }
            }

            if(IsKeyPressed(KEY_ESCAPE)) {
                currentShape.clear();
                drawingMode = false;
            }
        } 
        else {
            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                selectedShape = rayCasting(shapes, (int)mousePos.x, (int)mousePos.y);
                for (int i = 0; i < shapes.size(); ++i) {
                    shapes[i].selected = (i == selectedShape);
                }
            }

            if (selectedShape != -1 && IsKeyPressed(KEY_B)) {
                if (shapes[selectedShape].shape == Polygon) {
                    robotPolygon = selectedShape;
                    std::cout << "Polígono " << robotPolygon << " definido como robô." << std::endl;
                }
            }
        }

        if(IsKeyPressed(KEY_D)) {
            drawingMode = !drawingMode;
            currentShape.clear();
            if (drawingMode) {
                std::cout << "Modo desenho ativado" << std::endl;
            } else {
                std::cout << "Modo seleção ativado" << std::endl;
            }
        }

        if (IsKeyPressed(KEY_SPACE)) {
            if (robotPolygon != -1) {
                minkowskiResults.clear();
                const auto& robot = shapes[robotPolygon].points;
                int calculations = 0;
                
                for (int i = 0; i < shapes.size(); ++i) {
                    if (i == robotPolygon || shapes[i].shape != Polygon) continue;
                    auto inicio = std::chrono::high_resolution_clock::now();
                    const auto& obstacle = shapes[i].points;
                    std::vector<Vector2> result = minkowskiSum(robot, obstacle);
                    auto fim = std::chrono::high_resolution_clock::now();

                    Vector2 obstacle_centroid = calculateCentroid(obstacle);
                    Vector2 result_centroid = calculateCentroid(result);
                    Vector2 translation = {
                        obstacle_centroid.x - result_centroid.x, 
                        obstacle_centroid.y - result_centroid.y
                    };

                    for (auto& point : result) {
                        point.x += translation.x;
                        point.y += translation.y;
                    }

                    double tempo_ms = std::chrono::duration<double, std::milli>(fim - inicio).count();
                    minkowskiResults.push_back(result);
                    
                    exportMinkowskiToCSV(robotPolygon, i, result, robot, obstacle);

                    std::ofstream out("custo_computacional.csv", std::ios::app);
                    if (out.is_open()) {
                        out << shapes.size() << "," << tempo_ms << "\n";
                        out.close();
                    }

                    calculations++;
                }
                
                showMinkowski = true;
                std::cout << "Soma de Minkowski calculada para " << calculations << " obstáculos e exportada para CSV." << std::endl;
            } else {
                std::cout << "Selecione um polígono e pressione 'B' para defini-lo como robô antes de calcular." << std::endl;
            }
        }


        if (IsKeyPressed(KEY_H)) showMinkowski = !showMinkowski;

        BeginDrawing();
        ClearBackground(BLACK);

        for (int i = 0; i < shapes.size(); ++i) {
            Color drawColor = shapes[i].color;
            if (i == robotPolygon) {
                drawColor = ORANGE;
            } else if (shapes[i].selected) {
                drawColor = GREEN;
            }

            if (shapes[i].shape == Polygon) {
                for (size_t j = 0; j < shapes[i].points.size(); j++) {
                    DrawLineV(shapes[i].points[j], shapes[i].points[(j + 1) % shapes[i].points.size()], drawColor);
                }
            }
        }

        if (showMinkowski) {
            for (const auto& poly : minkowskiResults) {
                for (size_t i = 0; i < poly.size(); i++) {
                    DrawLineV(poly[i], poly[(i + 1) % poly.size()], YELLOW);
                }
            }
        }
        
        if (drawingMode) {
            for (size_t i = 0; i < currentShape.size(); i++) {
                DrawCircleV(currentShape[i], 5, YELLOW);
                if (i > 0) DrawLineV(currentShape[i-1], currentShape[i], YELLOW);
            }
            if (!currentShape.empty()) DrawLineV(currentShape.back(), mousePos, YELLOW);
        }

        std::string statusText = "Modo: ";
        statusText += drawingMode ? "Desenho (N para criar, ESC para cancelar)" : "Seleção (Clique para selecionar, B para definir robô)";
        statusText += " | SPACE para calcular Minkowski | H para mostrar/esconder";
        if (robotPolygon != -1) statusText += " | Robô: " + std::to_string(robotPolygon);
        
        DrawText(statusText.c_str(), 20, 20, 20, GREEN);
        
        if (showMinkowski && !minkowskiResults.empty()) {
            DrawText(("Resultados Minkowski: " + std::to_string(minkowskiResults.size()) + " obstáculos").c_str(), 20, 45, 20, YELLOW);
        }

        EndDrawing();
    }

    exportSessionData();
    CloseWindow();
    return 0;
}