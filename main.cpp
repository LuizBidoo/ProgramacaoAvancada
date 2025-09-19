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

// struct Point
// {
//     int x;
//     int y;
//     Color color;

// };

struct Polygon 
{
    std::vector<SDL_Point> points;
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
        const auto& poly = polygons[i];
        int num_points = poly.points.size();
        
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
    
    std::vector<Polygon> polygons;

    Polygon polygon_points;
    polygon_points.points.push_back({100, 100});
    polygon_points.points.push_back({200, 50});
    polygon_points.points.push_back({300, 100});
    polygon_points.points.push_back({250, 200});
    polygon_points.points.push_back({150, 200});
    polygon_points.points.push_back(polygon_points.points[0]);

    polygons.push_back(polygon_points);

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
                }
            }
        }

        if(draw == true) {
            // Limpa a tela
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
            SDL_RenderClear(renderer);
            SDL_RenderPresent(renderer);
            // Desenha as formas
            SDL_SetRenderDrawColor(renderer, 255,0,0,255);
            SDL_RenderDrawLines(renderer, polygon_points.points.data(), polygon_points.points.size());
            SDL_RenderPresent(renderer);
            draw = false;
        }

        // quando eu clico na tela preciso verificar se foi em algum polígono
        if(click == true) {
            std::cout << xPosition << std::endl;
            std::cout << yPosition << std::endl;
            int selectedPolygon = rayCasting(polygons, xPosition, yPosition);
            std::cout << "Polígono: " << selectedPolygon << std::endl;
            click = false;
        }
    }

    SDL_DestroyWindow(window);
    SDL_DestroyRenderer(renderer);
    SDL_Quit();

    return 0;
}