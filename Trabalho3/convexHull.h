#ifndef CONVEXHULL_H

#define CONVEXHULL_H

#include "raylib.h"
#include <vector>

bool compareX(Vector2& a, Vector2& b);
float cross(Vector2& A, Vector2& B, Vector2& O);
std::vector<Vector2> convexHull(std::vector<Vector2> pontos);

#endif