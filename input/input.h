#ifndef INPUT_H
#define INPUT_H

#include <SDL2/SDL.h>
#include <vector>
#include "../polygons/polygon.h"

bool processEvents(SDL_Event &event, bool &quit, bool &draw, bool &test, bool &click, 
                  Sint32 &xPosition, Sint32 &yPosition, std::vector<Polygon> &polygons);

#endif