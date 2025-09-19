#ifndef GEOMETRY_H
#define GEOMETRY_H

#include <SDL2/SDL.h>
#include <vector>
#include "../polygons/polygon.h"

bool pointOnSegment(const SDL_Point &a, const SDL_Point &b, Sint32 x, Sint32 y);
int rayCasting(std::vector<Polygon> &polygons, Sint32 x, Sint32 y);

#endif