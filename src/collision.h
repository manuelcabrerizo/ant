#pragma once

class Triangle;
class Sphere;
class Cylinder;

class Ray
{
public:
     void Init(vec3 o, vec3 d);
     bool Intersect(Triangle& triangle, f32& u, f32& v, f32& w, f32& t);
     bool Intersect(Sphere& sphere, f32& t);
     
     vec3 o;
     vec3 d;
};

class Segment
{
public:
     void Init(vec3 a, vec3 b);
     bool Intersect(Triangle& triangle, f32& u, f32& v, f32& w, f32& t);
     bool Intersect(Sphere& sphere, f32& t);
     bool Intersect(Cylinder& cylinder, f32& t);
     vec3 ClosestPoint(vec3 point, f32& t);
     
     vec3 a;
     vec3 b;
};

class Plane
{
public:

     void Init(vec3 n, f32 d);
     void Init(Triangle &triangle);

     bool Intersect(Ray& ray, f32& t);
     bool Intersect(Segment& segment, f32& t);

     vec3 ClosestPoint(vec3 q);
     
     vec3 n;
     f32 d;
};

class Sphere
{
public:
     void Init(vec3 c, f32 r);

     bool Intersect(Ray& ray, f32& t);
     bool Intersect(Segment& segment, f32& t);
     bool Intersect(Triangle& triangle, vec3& n, f32& penetration);
     
     bool DynamicIntersect(Plane& plane, vec3 movement, f32& t);
     bool DynamicIntersect(Triangle& triangle, vec3 movement, f32& u, f32& v, f32& w,
                           f32& t, vec3& n);
     
     vec3 c;
     f32 r;
};

class Cylinder
{
public:
     void Init(vec3 p, vec3 q, f32 r);
     
     vec3 p;
     vec3 q;
     f32 r;
};

class Capsule
{
public:

     vec3 p;
     vec3 q;
     f32 r;
};

class AABB
{
public:

     vec3 min;
     vec3 max;
};

class OBB
{
public:

     vec3 c;
     vec3 u[3];
     vec3 e;
};


class Triangle
{
public:
     void Init(vec3 a, vec3 b, vec3 c);
     bool Intersect(Ray& ray, f32& u, f32& v, f32& w, f32& t);
     bool Intersect(Segment& segment, f32& u, f32& v, f32& w, f32& t);

     bool PointInside(vec3 q, f32& u, f32& v, f32& w);
     vec3 ClosestPoint(vec3 q);

     vec3 a, b, c;
     vec3  n;
};

#define MAX_COLLISION_COUNT 16

struct CollisionData
{
     vec3 n;
     f32 penetration;
     f32 t;
     f32 u, v, w;
};

class CollisionWorld
{
public:
     void LoadFromFile(const char *);

     bool Intersect(Segment& segment, f32& t, vec3& n);
     bool Intersect(Ray& ray, f32& t, vec3& n);

     bool Intersect(Sphere& sphere, Array<CollisionData>& collisionData);
     
     bool DynamicIntersect(Sphere& sphere, vec3 movement, Array<CollisionData>& collisionData);

private:
     Array<Triangle> triangles;    
};
