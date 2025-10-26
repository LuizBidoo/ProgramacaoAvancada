#include "raylib.h"
#include "convexHull.h"
#include <vector>
#include <iostream>
#include <cmath>
#include <algorithm>

void reorder_polygon(std::vector<Vector2> & P) {
    if (P.empty()) return;
    
    size_t pos = 0;
    for (size_t i = 1; i < P.size(); i++) {
        if (P[i].y < P[pos].y || (P[i].y == P[pos].y && P[i].x < P[pos].x))
            pos = i;
    }
    std::rotate(P.begin(), P.begin() + pos, P.end());
}

float cross(const Vector2 &O, const Vector2 &A, const Vector2 &B) {
    return (A.x - O.x) * (B.y - O.y) - (A.y - O.y) * (B.x - O.x);
}

Vector2 pointSum(const Vector2 &A, const Vector2 &B) {
    return {A.x + B.x, A.y + B.y};
}

Vector2 pointSub(const Vector2 &A, const Vector2 &B) {
    return {A.x - B.x, A.y - B.y};
}

std::vector<Vector2> minkowskiSum(const std::vector<Vector2> &P, const std::vector<Vector2> &Q) {
    if (P.empty() || Q.empty()) return {};
    
    std::vector<Vector2> vertex_sums;
    vertex_sums.reserve(P.size() * Q.size());

    for (const auto& p_v : P) {
        for (const auto& q_v : Q) {
            vertex_sums.push_back(pointSum(p_v, q_v));
        }
    }

    return convexHull(vertex_sums);
}
