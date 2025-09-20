#include "graphics/graphics.h"
#include "input/input.h"
#include "polygons/polygon.h"
#include "geometry/geometry.h"
#include <iostream>
#include <cstdlib>
#include <ctime>

int main() {
    srand(static_cast<unsigned int>(time(0)));
    
    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;
    
    if (!initSDL(window, renderer)) {
        return -1;
    }
    
    SDL_Event event;
    bool quit = false;
    bool draw = false;
    bool click = false;
    bool creatingPolygon = false;
    bool done = false;
    bool test = false;

    // variável que guarda os possíveis pontos do possível polígono kkk
    std::vector<SDL_Point> currentPolygonPoints;
    Color currentPolygonColor;

    
    std::vector<Polygon> polygons;
    Sint32 xPosition = 0;
    Sint32 yPosition = 0;
    
    while(!quit) {
        processEvents(event, quit, creatingPolygon, done, test, click, xPosition, yPosition, currentPolygonPoints, currentPolygonColor);
        
        if(draw) {
            renderScene(renderer, polygons);
            draw = false;
        }
        
        if(test) {
            for(size_t i = 0; i < polygons.size(); i++) {
                std::cout << "Shape: " << i << std::endl;
                polygons[i].toString();
            }
            test = false;
        }

        if(creatingPolygon) {
            if(done) {
                currentPolygonPoints.push_back(currentPolygonPoints[0]);
                Polygon createdPolygon;
                createdPolygon.points = currentPolygonPoints;
                createdPolygon.color = currentPolygonColor;
                polygons.push_back(createdPolygon);
                done = false;
                creatingPolygon = false;
                draw = true;
            }
            if (click) {
                std::cout << xPosition << std::endl;
                std::cout << yPosition << std::endl;
                currentPolygonPoints.push_back({xPosition, yPosition});
                click = false;
            }
        }

        // if(click) {
        //     int option;
        //     std::cout << "Quer desenhar ou mudar algum formato?" << std::endl;
        //     std::cout << "1. Desenhar\n2. Mudar" << std::endl;
        //     std::cin >> option;
            
        //     switch(option) {

        //         case 1:
        //         {
        //             Polygon createdPolygon;
        //             int shape;
        //             std::cout << "Quantos lados terão a sua forma?\n";
        //             std::cin >> shape;
                    
        //             for(int i = 0; i < shape; i++) {
        //                 int x, y;
        //                 std::cin >> x >> y;
        //                 createdPolygon.points.push_back({x,y});
        //             }
        //             createdPolygon.points.push_back(createdPolygon.points[0]);
                    
        //             Sint32 r,g,b;
        //             std::cout << "Cores (R G B): " << std::endl;
        //             std::cin >> r >> g >> b;
        //             createdPolygon.color.red = r;
        //             createdPolygon.color.green = g;
        //             createdPolygon.color.blue = b;
                    
        //             polygons.push_back(createdPolygon);
        //             draw = true;
        //             break;
        //         }
        //         case 2:
        //         {
        //             int selectedPolygon = rayCasting(polygons, xPosition, yPosition);
        //             if (selectedPolygon == -1) {
        //                 std::cout << "Nenhum polígono selecionado!" << std::endl;
        //                 break;
        //             }
                    
        //             Polygon &poly = polygons[selectedPolygon];
        //             for(size_t i = 0; i < poly.points.size() - 1; i++) {
        //                 std::cout << "Ponto " << i << ": " << poly.points[i].x << " " << poly.points[i].y << std::endl;
        //                 std::cout << "Novas coordenadas (x y): ";
                        
        //                 int newX, newY;
        //                 std::cin >> newX >> newY;
        //                 poly.points[i].x = newX;
        //                 poly.points[i].y = newY;
        //             }
        //             poly.points.pop_back();
        //             poly.points.push_back(poly.points[0]);
        //             draw = true;
        //             break;
        //         }
        //     }
        //     click = false;
        // }
    }
    
    cleanupSDL(window, renderer);
    return 0;
}