#ifndef POLYGON_H
#define POLYGON_H

#include <SDL2/SDL.h>
#include <vector>
#include <iostream>

struct Color
{
    Uint8 red;
    Uint8 green;
    Uint8 blue;
    
    Color();
};

struct Polygon 
{
    std::vector<SDL_Point> points;
    Color color;

    void toString();
};

#endif