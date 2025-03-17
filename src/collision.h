#pragma once

class Triangle;

class Ray
{
public:
     void Init(vec3 o, vec3 d);
     bool Intersect(Triangle& triangle, f32& u, f32& v, f32& w, f32& t);
     
     vec3 o;
     vec3 d;
};

class Segment
{
public:
     void Init(vec3 a, vec3 b);
     bool Intersect(Triangle& triangle, f32& u, f32& v, f32& w, f32& t);

     vec3 a;
     vec3 b;
};

class Sphere
{
public:

     vec3 c;
     f32 r;
};

class Capsule
{
public:

     vec3 a;
     vec3 b;
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

class Plane
{
public:

     void Init(vec3 n, f32 d);
     void Init(Triangle &triangle);

     bool Intersect(Ray& ray, f32& t);
     bool Intersect(Segment& segment, f32& t);
     
     vec3 n;
     f32 d;
};

class Triangle
{
public:
     void Init(vec3 a, vec3 b, vec3 c);
     bool Intersect(Ray& ray, f32& u, f32& v, f32& w, f32& t);
     bool Intersect(Segment& segment, f32& u, f32& v, f32& w, f32& t);
     bool Intersect(Sphere& sphere, f32& u, f32& v, f32& w, f32& t);
     bool Intersect(Capsule& capsule, f32& u, f32& v, f32& w, f32& t);
     bool PointInside(vec3 q, f32& u, f32& v, f32& w);

     vec3 a, b, c;
     vec3  n;
};

class CollisionWorld
{
public:
     void LoadFromFile(const char *);

     bool Intersect(Segment& segment, f32& t, vec3& n);
     bool Intersect(Ray& ray, f32& t, vec3& n);
private:
     Array<Triangle> triangles;    
};
