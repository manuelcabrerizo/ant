#pragma once

#include <common.h>
#include <math/vector3.h>

class Cylinder
{
private:
    Vector3 p;
    Vector3 q;
    float r;
public:
    void Init(Vector3 p, Vector3 q, f32 r);
    
    Vector3 GetP() const;
    Vector3 GetQ() const;
    float GetRadio() const;
    
    bool Intersect(const Cylinder& cylinder) const;

    Vector3 ClosestPoint(const Vector3& point) const;
    float SqDistPoint(const Vector3& point) const;

    void DebugDraw(int subdivision, const Vector3& color);
};