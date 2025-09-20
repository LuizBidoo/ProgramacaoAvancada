#include "input.h"
#include "../geometry/geometry.h"
#include "../polygons/polygon.h"
#include <iostream>

bool processEvents(SDL_Event &event, bool &quit, bool &creatingPolygon, bool &done, bool &test, bool &click, 
                  Sint32 &xPosition, Sint32 &yPosition, std::vector<SDL_Point> &currentPolygonPoints, Color &currentPolygonColor) {
    while (SDL_PollEvent(&event)) {
        if(event.type == SDL_QUIT) {
            quit = true;
            return true;
        }
        
        if(event.type == SDL_MOUSEBUTTONDOWN) {
            if(event.button.button == SDL_BUTTON_LEFT) {
                xPosition = event.button.x;
                yPosition = event.button.y;
                click = true;
                return true;
            }
        }
        
        if(event.type == SDL_KEYDOWN) {
            switch(event.key.keysym.sym) {
                case SDLK_ESCAPE:
                    quit = true;
                    return true;
                case SDLK_t:
                    creatingPolygon = false;
                    test = true;
                    return true;
                case SDLK_p:
                    done = true;
                    return true;
                case SDLK_c: // modo de criação
                    if(!creatingPolygon) { // entra no modo de criação limpando os dados anteriores
                        creatingPolygon = true;
                        currentPolygonPoints.clear();
                        currentPolygonColor.red = (rand() % 255); 
                        currentPolygonColor.green = (rand() % 255);
                        currentPolygonColor.blue = (rand() % 255);
                        std::cout << "modo de criação ativado, clique na tela";
                    }
                return true;
            }
        }
    }
    return false;
}