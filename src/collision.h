#pragma once

#include <common.h>
#include <containers.h>
#include <math/vector3.h>

#include <collisions/segment.h>
#include <collisions/ray.h>
#include <collisions/aabb.h>
#include <collisions/obb.h>
#include <collisions/plane.h>
#include <collisions/sphere.h>
#include <collisions/cylinder.h>
#include <collisions/capsule.h>
#include <collisions/triangle.h>

#define MAX_COLLISION_COUNT 16

struct CollisionData
{
    Vector3 point;
    Vector3 n;
    f32 penetration;
    f32 t;
};

class CollisionWorld
{
public:
    void AddOBB(const OBB& obb);
    void AddTriangle(const Triangle& triangle);

    void LoadFromFile(const char *);
    bool Intersect(Segment& segment, f32& t, Vector3& n);
    bool Intersect(Ray& ray, f32& t, Vector3& n);
    bool Intersect(Sphere& sphere, Array<CollisionData>& collisionData);
    bool Intersect(const Capsule& capsule, Array<CollisionData>& collisionData);
    bool Intersect(const OBB& obb, Array<CollisionData>& collisionData);


    bool DynamicIntersect(Sphere& sphere, Vector3 movement, Array<CollisionData>& collisionData);
private:
    Array<Triangle> triangles;  
    Array<AABB> aabbs;
    Array<OBB> obbs;
    Array<Sphere> spheres;
    Array<Capsule> capsules;
};


