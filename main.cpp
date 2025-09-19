#include <SDL2/SDL.h>
#include <cmath>
#include <cstdlib>
#include <vector>
#include <iostream>
#include <algorithm>

struct Color
{
    Uint8 red = (rand() % 255) - 1;
    Uint8 green = (rand() % 255) - 1;
    Uint8 blue = (rand() % 255) - 1;
};

struct Polygon 
{
    std::vector<SDL_Point> points;
    Color color;

    void toString() {
        for(int i = 0; i < this->points.size(); i++) {
            std::cout << "Ponto " << i << std::endl;
            std::cout << this->points[i].x << std::endl;
            std::cout << this->points[i].y << std::endl;
            std::cout << std::endl;
        }
    }
};

// Verificando se ponto está em um segmento
bool pointOnSegment(const SDL_Point &a, const SDL_Point &b, Sint32 x, Sint32 y) {
    SDL_Point p = {x, y};
    
    // Verificando colinearidade
    int cross = (p.x - a.x) * (b.y - a.y) - (p.y - a.y) * (b.x - a.x);
    if (abs(cross) > 1e-9) {
        return false;
    }
    
    bool within_x = (x >= std::min(a.x, b.x) && x <= std::max(a.x, b.x));
    bool within_y = (y >= std::min(a.y, b.y) && y <= std::max(a.y, b.y));
    
    return within_x && within_y;
}

// Algoritmo de ray casting
int rayCasting(std::vector<Polygon> &polygons, Sint32 x, Sint32 y) {
    for (int i = 0; i < polygons.size(); i++) {
        const auto& poly = polygons[i]; // ref pro poligono
        int num_points = poly.points.size(); // pontos do polígono
        
        if (num_points < 3) continue; // caso base tem que ser um polígono
        
        bool inside = false;
        
        for (int j = 0, k = num_points - 1; j < num_points; k = j++) {
            const SDL_Point& p1 = poly.points[j];
            const SDL_Point& p2 = poly.points[k];
            
            // verificando se ponto está na borda
            if (pointOnSegment(p1, p2, x, y)) {
                return i;
            }
            
            if (((p1.y > y) != (p2.y > y)) && 
                (x < (p2.x - p1.x) * (y - p1.y) / (p2.y - p1.y) + p1.x)) {
                inside = !inside;
            }
        }
        
        if (inside) {
            return i;
        }
    }
    return -1;
}
int main(int argc, char* argv[]) {
    srand(static_cast<unsigned int>(time(0)));
    SDL_Window * window = nullptr;
    SDL_Renderer * renderer = nullptr;


    SDL_Init(SDL_INIT_VIDEO);
    SDL_CreateWindowAndRenderer(800, 600, 0, &window, &renderer);

    // verificação de erros:

    if(!window) {
        SDL_Log("Erro ao criar a janela");
        return -1;
    } if(!renderer) {
        SDL_Log("Erro ao criar o renderer");
        return -1;
    }

    SDL_SetWindowTitle(window, "Trabalho 1");

    SDL_SetRenderDrawColor(renderer, 0,0,0,255); // setando a cor do renderer
    SDL_RenderClear(renderer); // limpando o renderer
    SDL_RenderPresent(renderer);

    SDL_Event event;
    bool quit = false;
    bool draw = false;
    bool click = false;
    bool test = false;
    
    std::vector<Polygon> polygons;

    Sint32 xPosition = 0;
    Sint32 yPosition = 0;

    while(!quit) {
        while (SDL_PollEvent(&event)) {
            if(event.type == SDL_QUIT) {
                quit = true;
            } if(event.type == SDL_MOUSEBUTTONDOWN) {
                if(event.button.button == SDL_BUTTON_LEFT) {
                    xPosition = event.button.x; // Coordenadas x e y para ponto médio
                    yPosition = event.button.y;
                    click = true; 
                }
            } if(event.type == SDL_KEYDOWN) {
                switch(event.key.keysym.sym) {
                    case SDLK_ESCAPE:
                        quit = true;
                        break;
                    case SDLK_d:
                        draw = true;
                        break;
                    case SDLK_t:
                        test = true;
                        break;
                }
            }
        }

        if(draw == true) {
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
            SDL_RenderClear(renderer);
            
            for(auto & polygon : polygons) {
                SDL_SetRenderDrawColor(renderer, polygon.color.red, polygon.color.green, polygon.color.blue, 255);
                SDL_RenderDrawLines(renderer, polygon.points.data(), polygon.points.size());
            }
            
            SDL_RenderPresent(renderer);
            draw = false;
        }

        if(test == true) {
            for(int i = 0; i < polygons.size(); i++) {
                std::cout << "Shape: " << i << std::endl;
                polygons[i].toString();
            }
            test = false;
        }

        // quando eu clico na tela preciso verificar se foi em algum polígono
        if(click == true) {
            int option;
            std::cout << "Quer desenhar ou mudar algum formato?" << std::endl;
            std::cout << "1. Desenhar\n 2. Mudar" << std::endl;
            std::cin >> option;
            switch(option) {
                case 1:
                {
                    Polygon createdPolygon;
                    int shape;
                    std::cout << "Quantos lados terão a sua forma?\n";
                    std::cin >> shape;
                    // pedindo os lados
                    for(int i = 0; i < shape; i++) {
                        int x, y;
                        std::cin >> x;
                        std::cin >> y;
                        createdPolygon.points.push_back({x,y});
                    }
                    createdPolygon.points.push_back(createdPolygon.points[0]); // fechando o formato

                    // Cores
                    Sint32 r,g,b;
                    std::cout << "Cores: " << std::endl;
                    std::cin >> r; 
                    createdPolygon.color.red = r;               
                    std::cin >> g;
                    createdPolygon.color.green = g;
                    std::cin >> b;
                    createdPolygon.color.blue = b;

                    // Colocando na lista de formas
                    polygons.push_back(createdPolygon);

                    break;
                }
                case 2:
                    int selectedPolygon = rayCasting(polygons, xPosition, yPosition);
                    while(selectedPolygon == -1) {
                        std::cout << "Por favor selecione uma forma" << std::endl;
                        click = false;
                    }
                    Polygon & poly = polygons[selectedPolygon];
                    for(int i = 0; i < poly.points.size() - 1; i++) { // deixar a pessoa selecionar os pontos
                        int pointX = poly.points[i].x;
                        int pointY = poly.points[i].y;
                        int newX = 0;
                        int newY = 0;
                        bool change;
                        std::cout << "O ponto que será mudado: " << std::endl;
                        std::cout << pointX << " " << pointY << std::endl;
                        std::cin >> newX; 
                        std::cin >> newY;
                        poly.points[i].x = newX;
                        poly.points[i].y = newY;
                    }
                    poly.points.pop_back();
                    poly.points.push_back(poly.points[0]);
                    break;
            }
            click = false;
        }
    }

    SDL_DestroyWindow(window);
    SDL_DestroyRenderer(renderer);
    SDL_Quit();

    return 0;
}