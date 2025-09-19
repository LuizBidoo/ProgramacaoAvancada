#include "polygon.h"
#include <cstdlib>
#include <ctime>

Color::Color() {
    red = (rand() % 255);
    green = (rand() % 255);
    blue = (rand() % 255);
}

void Polygon::toString() {
    for(size_t i = 0; i < this->points.size(); i++) {
        std::cout << "Ponto " << i << std::endl;
        std::cout << this->points[i].x << std::endl;
        std::cout << this->points[i].y << std::endl;
        std::cout << std::endl;
    }
}