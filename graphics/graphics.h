#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <SDL2/SDL.h>
#include <vector>
#include "../polygons/polygon.h"

bool initSDL(SDL_Window* &window, SDL_Renderer* &renderer);
void cleanupSDL(SDL_Window* window, SDL_Renderer* renderer);
void renderScene(SDL_Renderer* renderer, std::vector<Polygon> &polygons);

#endif