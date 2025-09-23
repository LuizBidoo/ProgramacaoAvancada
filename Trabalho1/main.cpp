#include "raylib/build/raylib/include/raylib.h"
#include <vector>
#include <iostream>
#include <cmath>
#include <algorithm>

enum Shape {
    Circle,
    RectangleShape,
    TriangleShape,
    Line
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
};

struct SessionInfo {

};


bool pointOnSegment(const Vector2 &a, const Vector2 &b, int x, int y) {
    Vector2 p = {(float)x, (float)y};
    
    float cross = (p.x - a.x) * (b.y - a.y) - (p.y - a.y) * (b.x - a.x);
    if (fabs(cross) > 1e-9) {
        return false;
    }
    
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
                if (pointInCircle({(float)shape.x1, (float)shape.y1}, shape.radius, x, y)) {
                    return i;
                }
                break;
                
            case RectangleShape:
                if (pointInRectangle(shape.x1, shape.y1, shape.width, shape.height, x, y)) {
                    return i;
                }
                break;
                
            case TriangleShape:
                if (pointInTriangle(shape.v1, shape.v2, shape.v3, x, y)) {
                    return i;
                }
                break;
                
            case Line:
                if (pointOnSegment({(float)shape.x1, (float)shape.y1}, {(float)shape.x2, (float)shape.y2}, x, y)) {
                    return i;
                }
                break;
        }
    }
    return -1;
}

int main() 
{
    InitWindow(800, 800, "Salve");
    SetTargetFPS(60);

    // Lista de formas
    std::vector<ShapeInfo> shapes;
    int selectedShape = -1;

    // Loop de execução
    while(WindowShouldClose() == false)
    {   
        // event handling
        if(IsKeyDown(KEY_C)) { // circulo
            int centerX;
            int centerY;
            float radius;
            Color color;
            std::cout << "Informações do Círculo" << std::endl;
            std::cin >> centerX >> centerY >> radius;
            std::cout << "Cores" << std::endl;
            int r, g, b;
            std::cin >> r >> g >> b;
            color.r = (unsigned char)r;
            color.g = (unsigned char)g;
            color.b = (unsigned char)b;
            color.a = 255;
            ShapeInfo shape {Circle, color};
            shape.x1 = centerX;
            shape.y1 = centerY;
            shape.radius = radius;
            shapes.push_back(shape);
        } else if(IsKeyDown(KEY_R)) {
            int posX, posY, width, height;
            Color color;
            std::cout << "Informações do Retangulo" << std::endl;
            std::cin >> posX >> posY >> width >> height;
            std::cout << "Cores" << std::endl;
            int r, g, b;
            std::cin >> r >> g >> b;
            color.r = (unsigned char)r;
            color.g = (unsigned char)g;
            color.b = (unsigned char)b;
            color.a = 255;
            ShapeInfo shape {RectangleShape, color};
            shape.x1 = posX;
            shape.y1 = posY;
            shape.width = width;
            shape.height = height;
            shapes.push_back(shape);
        } else if(IsKeyDown(KEY_T)) {
            Vector2 v1, v2, v3;
            Color color;
            std::cout << "Informações do Triangulo (v1.x v1.y v2.x v2.y v3.x v3.y):" << std::endl;
            std::cin >> v1.x >> v1.y >> v2.x >> v2.y >> v3.x >> v3.y;
            std::cout << "Cores" << std::endl;
            int r, g, b;
            std::cin >> r >> g >> b;
            color.r = (unsigned char)r;
            color.g = (unsigned char)g;
            color.b = (unsigned char)b;
            color.a = 255;
            ShapeInfo shape {TriangleShape, color};
            shape.v1 = v1;
            shape.v2 = v2;
            shape.v3 = v3;
            shapes.push_back(shape);
        } else if(IsKeyDown(KEY_L)) {
            int x1, y1, x2, y2;
            Color color;
            std::cout << "Informações da Linha (v1.x v1.y, v2.x v2.y):" << std::endl;
            std::cin >> x1 >> y1 >> x2 >> y2;
            std::cout << "Cores" << std::endl;
            int r, g, b;
            std::cin >> r >> g >> b;
            color.r = (unsigned char)r;
            color.g = (unsigned char)g;
            color.b = (unsigned char)b;
            color.a = 255;
            ShapeInfo shape {Line, color};
            shape.x1 = x1;
            shape.y1 = y1;
            shape.x2 = x2;
            shape.y2 = y2;
            shapes.push_back(shape);
        } if(IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            Vector2 mousePos = GetMousePosition();
            selectedShape = rayCasting(shapes, mousePos.x, mousePos.y);
            if (selectedShape != -1) {
                std::cout << "Shape selecionado: " << selectedShape << std::endl;
                
                // Primeiro: perguntar se quer mudar posição ou outras propriedades
                std::cout << "O que deseja modificar?" << std::endl;
                std::cout << "1 - Posição" << std::endl;
                std::cout << "2 - Propriedades específicas" << std::endl;
                int opcao;
                std::cin >> opcao;
                
                if (opcao == 1) {
                    // Mudar posição
                    std::cout << "Digite nova posição (x y): ";
                    int newX, newY;
                    std::cin >> newX >> newY;
                    
                    switch (shapes[selectedShape].shape) {
                        case Circle:
                        case RectangleShape:
                        case Line:
                            // Para círculo, retângulo e linha, mudamos x1 e y1
                            shapes[selectedShape].x1 = newX;
                            shapes[selectedShape].y1 = newY;
                            break;
                            
                        case TriangleShape:
                            // Para triângulo, calculamos o deslocamento e movemos todos os vértices
                            Vector2 centroAtual = {
                                (shapes[selectedShape].v1.x + shapes[selectedShape].v2.x + shapes[selectedShape].v3.x) / 3,
                                (shapes[selectedShape].v1.y + shapes[selectedShape].v2.y + shapes[selectedShape].v3.y) / 3
                            };
                            Vector2 deslocamento = {newX - centroAtual.x, newY - centroAtual.y};
                            
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
                    // Mudar propriedades específicas (código original)
                    switch (shapes[selectedShape].shape) {
                        case Circle:
                            std::cout << "Mudar Círculo" << std::endl;
                            std::cout << "Digite novo raio: ";
                            std::cin >> shapes[selectedShape].radius;
                            std::cout << "Digite novas cores (R G B): ";
                            int r, g, b;
                            std::cin >> r >> g >> b;
                            shapes[selectedShape].color.r = (unsigned char)r;
                            shapes[selectedShape].color.g = (unsigned char)g;
                            shapes[selectedShape].color.b = (unsigned char)b;
                            break;
                            
                        case RectangleShape:
                            std::cout << "Mudar Retângulo" << std::endl;
                            std::cout << "Digite nova largura e altura: ";
                            std::cin >> shapes[selectedShape].width >> shapes[selectedShape].height;
                            std::cout << "Digite novas cores (R G B): ";
                            std::cin >> r >> g >> b;
                            shapes[selectedShape].color.r = (unsigned char)r;
                            shapes[selectedShape].color.g = (unsigned char)g;
                            shapes[selectedShape].color.b = (unsigned char)b;
                            break;
                            
                        case TriangleShape:
                            std::cout << "Mudar Triângulo" << std::endl;
                            std::cout << "Digite novos vértices (v1.x v1.y v2.x v2.y v3.x v3.y): ";
                            std::cin >> shapes[selectedShape].v1.x >> shapes[selectedShape].v1.y 
                                    >> shapes[selectedShape].v2.x >> shapes[selectedShape].v2.y 
                                    >> shapes[selectedShape].v3.x >> shapes[selectedShape].v3.y;
                            std::cout << "Digite novas cores (R G B): ";
                            std::cin >> r >> g >> b;
                            shapes[selectedShape].color.r = (unsigned char)r;
                            shapes[selectedShape].color.g = (unsigned char)g;
                            shapes[selectedShape].color.b = (unsigned char)b;
                            break;
                            
                        case Line:
                            std::cout << "Mudar Linha" << std::endl;
                            std::cout << "Digite novas coordenadas (x1 y1 x2 y2): ";
                            std::cin >> shapes[selectedShape].x1 >> shapes[selectedShape].y1 
                                    >> shapes[selectedShape].x2 >> shapes[selectedShape].y2;
                            std::cout << "Digite novas cores (R G B): ";
                            std::cin >> r >> g >> b;
                            shapes[selectedShape].color.r = (unsigned char)r;
                            shapes[selectedShape].color.g = (unsigned char)g;
                            shapes[selectedShape].color.b = (unsigned char)b;
                            break;
                    }
                }
            }
        }
        // updating positions


        // drawing
        BeginDrawing();
        ClearBackground(BLACK);
        for(size_t i = 0; i < shapes.size(); i ++) {
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
        EndDrawing();
    }

    CloseWindow();
    return 0;
}

