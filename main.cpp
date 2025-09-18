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

bool pointIntersects(const SDL_Point & a, const SDL_Point & b, Sint32 x, Sint32 y) {
    SDL_Point p;
    p.x = x;
    p.y = y;
    bool cross_product = (p.x - a.x) * (b.y - a.y) - (p.y - a.y) * (b.x - a.x);
    if (abs(cross_product) > 1e-9) { 
        return false;
    }
    bool within_x = (p.x >= std::min(a.x, b.x) && p.x <= std::max(a.x, b.x));
    bool within_y = (p.y >= std::min(a.y, b.y) && p.y <= std::max(a.y, b.y));

    return within_x && within_y;
}

// algoritmo de ray casting para verificar se onde clicamos está dentro do polígono
int rayCasting(std::vector<Polygon> & polygons, Sint32 x, Sint32 y ) {
    for(size_t i = 0; i < polygons.size(); i++) {
        int count = 0;
        size_t num_points = polygons[i].points.size(); // numero de pontos do meu polígono
        
        if (num_points < 2) { // caso que é apenas um ponto
            continue;
        }
        
        for(size_t j = 0; j < num_points; j++) {
            size_t next_j = (j + 1) % num_points; // 
            
            if(pointIntersects(polygons[i].points[j], polygons[i].points[next_j], x, y)) {
                count++;
            }
        }
        if(count % 2 != 0) {
            return (int)i;
        }
    }
    return -1; // Note: This returns a large positive number since size_t is unsigned
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