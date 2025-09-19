#include "graphics.h"
#include <iostream>

bool initSDL(SDL_Window* &window, SDL_Renderer* &renderer) {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        std::cout << "SDL_Init Error: " << SDL_GetError() << std::endl;
        return false;
    }
    
    SDL_CreateWindowAndRenderer(800, 600, 0, &window, &renderer);
    
    if (!window) {
        std::cout << "Erro ao criar a janela" << std::endl;
        return false;
    }
    
    if (!renderer) {
        std::cout << "Erro ao criar o renderer" << std::endl;
        return false;
    }
    
    SDL_SetWindowTitle(window, "Trabalho 1");
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
    SDL_RenderPresent(renderer);
    
    return true;
}

void cleanupSDL(SDL_Window* window, SDL_Renderer* renderer) {
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

void renderScene(SDL_Renderer* renderer, std::vector<Polygon> &polygons) {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
    
    for(auto & polygon : polygons) {
        SDL_SetRenderDrawColor(renderer, polygon.color.red, polygon.color.green, polygon.color.blue, 255);
        SDL_RenderDrawLines(renderer, polygon.points.data(), polygon.points.size());
    }
    
    SDL_RenderPresent(renderer);
}