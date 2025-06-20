#include "collision_utils.h"

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
