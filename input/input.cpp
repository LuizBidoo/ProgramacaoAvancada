#include "input.h"
#include "../geometry/geometry.h"
#include <iostream>

bool processEvents(SDL_Event &event, bool &quit, bool &draw, bool &test, bool &click, 
                  Sint32 &xPosition, Sint32 &yPosition, std::vector<Polygon> &polygons) {
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
                case SDLK_d:
                    draw = true;
                    return true;
                case SDLK_t:
                    test = true;
                    return true;
            }
        }
    }
    return false;
}