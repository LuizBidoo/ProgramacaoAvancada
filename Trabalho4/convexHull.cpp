#include "raylib.h"
#include <vector>
#include <iostream>
#include <cmath>
#include <algorithm>

bool compareX(Vector2& a, Vector2& b) {
    if (a.x == b.x) { // caso de empate
        return a.y < b.y;
    }
    return a.x < b.x;
}

float cross(Vector2& A, Vector2& B, Vector2& O) {
    return (A.x - O.x) * (B.y - O.y) - (A.y - O.y) * (B.x - O.x); 
}

std::vector<Vector2> convexHull(std::vector<Vector2> pontos) {

    if (pontos.size() <= 1) return pontos;

    std::sort(pontos.begin(), pontos.end(), compareX);


    std::vector<Vector2> lowerH;
    std::vector<Vector2> upperH;

    for(int i = 0; i < pontos.size(); i++) {
        while(lowerH.size() >= 2 && cross(lowerH[lowerH.size() - 2], lowerH.back(), pontos[i]) <= 0) {
            lowerH.pop_back();
        }
        lowerH.push_back(pontos[i]);
    }

    for(int i = static_cast<int>(pontos.size()) - 1; i >= 0; i--) {
        while(upperH.size() >= 2 && cross(upperH[upperH.size() - 2], upperH.back(), pontos[i]) <= 0) {
            upperH.pop_back();
        }
        upperH.push_back(pontos[i]);
    }

    if (!lowerH.empty()) lowerH.pop_back();
    if (!upperH.empty()) upperH.pop_back();

    lowerH.insert(lowerH.end(), upperH.begin(), upperH.end());

    return lowerH;
}