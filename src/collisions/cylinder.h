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
    
    void DebugDraw(int subdivision, const Vector3& color);

};