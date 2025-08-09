#pragma once

#include <common.h>
#include <containers.h>
#include <math/vector3.h>

struct CollisionData;

class CollisionUtils
{
public:
    static void ExtremePointsAlongDirection(const Vector3& dir, Array<Vector3>& points, i32* imin, i32* imax);
    static void SortCollisionByTime(Array<CollisionData>& arr);
    static void SortCollisionByPenetration(Array<CollisionData>& arr);
};
