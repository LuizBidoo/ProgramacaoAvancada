#ifndef MINKOWSKI_h

#define MINKOWSKI_h

#include "raylib.h"
#include <vector>

void reorder_polygon(std::vector<Vector2> & P);
float cross(Vector2& A, Vector2& B, Vector2& O);
Vector2 pointSum(const Vector2 &A, const Vector2 &B);
Vector2 pointSub(const Vector2 &A, const Vector2 &B);
std::vector<Vector2> minkowskiSum(const std::vector<Vector2> &P, const std::vector<Vector2> &Q);

#endif