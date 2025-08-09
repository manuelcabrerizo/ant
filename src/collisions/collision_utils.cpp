#include "collision_utils.h"
#include <collision.h>
#include <float.h>

void CollisionUtils::ExtremePointsAlongDirection(const Vector3& dir, Array<Vector3>& points, i32* imin, i32* imax)
{
    f32 minproj = FLT_MAX;
    f32 maxproj = -FLT_MAX;
    for (i32 i = 0; i < points.size; ++i)
    {
        // Project vector from origin to point onto direction vector
        f32 proj = points[i].Dot(dir);
        // Keep track of least distant point along direction vector
        if (proj < minproj)
        {
            minproj = proj;
            *imin = i;
        }
        // Keep track of most distant point along direction vector
        if (proj > maxproj)
        {
            maxproj = proj;
            *imax = i;
        }
    }
}

void CollisionUtils::SortCollisionByTime(Array<CollisionData>& arr)
{
    for (i32 i = 1; i < arr.size; ++i)
    {
        auto key = arr[i];
        i32 j = i - 1;

        while (j >= 0 && arr[j].t > key.t)
        {
            arr[j + 1] = arr[j];
            j = j - 1;
        }
        arr[j + 1] = key;
    }
}

void CollisionUtils::SortCollisionByPenetration(Array<CollisionData>& arr)
{
    for (i32 i = 1; i < arr.size; ++i)
    {
        auto key = arr[i];
        i32 j = i - 1;

        while (j >= 0 && arr[j].penetration < key.penetration)
        {
            arr[j + 1] = arr[j];
            j = j - 1;
        }
        arr[j + 1] = key;
    }
}

