#pragma once

#include <containers.h>
#include <math/vector3.h>

#define MAX_COLLISION_COUNT 16

struct CollisionData
{
    Vector3 point;
    Vector3 n;
    float penetration;
    float t;
};

class CollisionUtils
{
public:
    static void ExtremePointsAlongDirection(const Vector3& dir, Array<Vector3>& points, i32* imin, i32* imax);
    static void SortCollisionByTime(Array<CollisionData>& arr);
    static void SortCollisionByPenetration(Array<CollisionData>& arr);
};
