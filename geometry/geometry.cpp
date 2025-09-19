#include "geometry.h"
#include <cmath>
#include <algorithm>

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

int rayCasting(std::vector<Polygon> &polygons, Sint32 x, Sint32 y) {
    for (size_t i = 0; i < polygons.size(); i++) {
        const auto& poly = polygons[i];
        int num_points = poly.points.size();
        
        if (num_points < 3) continue;
        
        bool inside = false;
        
        for (int j = 0, k = num_points - 1; j < num_points; k = j++) {
            const SDL_Point& p1 = poly.points[j];
            const SDL_Point& p2 = poly.points[k];
            
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