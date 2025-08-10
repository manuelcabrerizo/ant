#pragma once

#include "collision_utils.h"

class Ray;
class Segment;

class Triangle
{
public:
    void Init(const Vector3& a, const Vector3& b, const Vector3& c);
    bool Intersect(const Ray& ray, float& t) const;
    bool Intersect(const Segment& segment, float& t) const;

    bool PointInside(const Vector3& q) const;

    // TODO: test others implementation
    Vector3 ClosestPoint(const Vector3& q) const;

    void DebugDraw(const Vector3& color);

    Vector3 a, b, c;
    Vector3  n;
};