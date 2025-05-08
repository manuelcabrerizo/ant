#pragma once

class Triangle;
class Sphere;
class Cylinder;

class Ray
{
public:
     void Init(Vector3 o, Vector3 d);
     bool Intersect(Triangle& triangle, f32& u, f32& v, f32& w, f32& t);
     bool Intersect(Sphere& sphere, f32& t);
     
     Vector3 o;
     Vector3 d;
};

class Segment
{
public:
     void Init(Vector3 a, Vector3 b);
     bool Intersect(Triangle& triangle, f32& u, f32& v, f32& w, f32& t);
     bool Intersect(Sphere& sphere, f32& t);
     bool Intersect(Cylinder& cylinder, f32& t);
     Vector3 ClosestPoint(Vector3 point, f32& t);
     
     Vector3 a;
     Vector3 b;
};

class Plane
{
public:

     void Init(Vector3 n, f32 d);
     void Init(Triangle &triangle);

     bool Intersect(Ray& ray, f32& t);
     bool Intersect(Segment& segment, f32& t);

     Vector3 ClosestPoint(Vector3 q);
     
     Vector3 n;
     f32 d;
};

class Sphere
{
public:
     void Init(Vector3 c, f32 r);

     bool Intersect(Ray& ray, f32& t);
     bool Intersect(Segment& segment, f32& t);
     bool Intersect(Triangle& triangle, Vector3& n, f32& penetration);
     
     bool DynamicIntersect(Plane& plane, Vector3 movement, f32& t);
     bool DynamicIntersect(Triangle& triangle, Vector3 movement, f32& u, f32& v, f32& w,
                           f32& t, Vector3& n);
     
     Vector3 c;
     f32 r;
};

class Cylinder
{
public:
     void Init(Vector3 p, Vector3 q, f32 r);
     
     Vector3 p;
     Vector3 q;
     f32 r;
};

class Capsule
{
public:
     Vector3 p;
     Vector3 q;
     f32 r;
};

class AABB
{
public:
     Vector3 min;
     Vector3 max;
};

class OBB
{
public:
     Vector3 c;
     Vector3 u[3];
     Vector3 e;
};


class Triangle
{
public:
     void Init(Vector3 a, Vector3 b, Vector3 c);
     bool Intersect(Ray& ray, f32& u, f32& v, f32& w, f32& t);
     bool Intersect(Segment& segment, f32& u, f32& v, f32& w, f32& t);

     bool PointInside(Vector3 q, f32& u, f32& v, f32& w);
     Vector3 ClosestPoint(Vector3 q);

     Vector3 a, b, c;
     Vector3  n;
};

#define MAX_COLLISION_COUNT 16

struct CollisionData
{
     Vector3 n;
     f32 penetration;
     f32 t;
     f32 u, v, w;
};

class CollisionWorld
{
public:
     void LoadFromFile(const char *);

     bool Intersect(Segment& segment, f32& t, Vector3& n);
     bool Intersect(Ray& ray, f32& t, Vector3& n);

     bool Intersect(Sphere& sphere, Array<CollisionData>& collisionData);
     
     bool DynamicIntersect(Sphere& sphere, Vector3 movement, Array<CollisionData>& collisionData);

private:
     Array<Triangle> triangles;    
};
