
void Ray::Init(vec3 o_, vec3 d_)
{
     o = o_;
     d = d_;
}

bool Ray::Intersect(Triangle& triangle, f32& u, f32& v, f32& w, f32& t)
{
     Plane plane;
     plane.Init(triangle);
     
     if(plane.Intersect(*this, t))
     {
          vec3 hitPos = o + d * t;
          return triangle.PointInside(hitPos, u, v, w);
     }
     return false;
}

bool Ray::Intersect(Sphere& sphere, f32& t)
{
     vec3 m = o - sphere.c;
     f32 b = dot(m, d);
     f32 c = dot(m, m) - sphere.r * sphere.r;

     if(c > 0.0f && b > 0.0f)
     {
          return false;
     }

     f32 discr = b*b - c;

     if(discr < 0.0f)
     {
          return false;
     }

     t = -b - sqrtf(discr);

     if(t < 0.0f)
     {
          t = 0.0f;
     }
     
     return true;
}


void Segment::Init(vec3 a_, vec3 b_)
{
     a = a_;
     b = b_;
}

bool Segment::Intersect(Triangle& triangle, f32& u, f32& v, f32& w, f32& t)
{
     Plane plane;
     plane.Init(triangle);

     if(plane.Intersect(*this, t))
     {
          vec3 hitPos = a + normalize(b - a) * t;
          return triangle.PointInside(hitPos, u, v, w);
     }
     return false;
}

bool Segment::Intersect(Sphere& sphere, f32& t)
{
     vec3 ab = b - a;
     vec3 o = a;
     vec3 d = normalize(ab);

     vec3 m = o - sphere.c;
     f32 b_ = dot(m, d);
     f32 c = dot(m, m) - sphere.r * sphere.r;

     if(c > 0.0f && b_ > 0.0f)
     {
          return false;
     }

     f32 discr = b_*b_ - c;

     if(discr < 0.0f)
     {
          return false;
     }

     t = -b_ - sqrtf(discr);

     if(t*t > dot(ab, ab))
     {
          return false;
     }

     if(t < 0.0f)
     {
          t = 0.0f;
     }
     
     return true;
     
}

#define EPSILON 0.001f

bool Segment::Intersect(Cylinder& cylinder, f32& t)
{
     vec3 d = cylinder.q - cylinder.p, m = a - cylinder.p, n = b - a;
     f32 md = dot(m, d);
     f32 nd = dot(n, d);
     f32 dd = dot(d, d);

     if(md < 0.0f && md + nd < 0.0f)
     {
          return false;
     }
     if(md > dd && md + nd > dd)
     {
          return false;
     }

     f32 nn = dot(n, n);
     f32 mn = dot(m, n);
     f32 a_ = dd * nn - nd * nd;
     f32 k = dot(m, m) - cylinder.r * cylinder.r;
     f32 c = dd * k - md * md;

     if(fabsf(a_) < EPSILON)
     {
          if(c > 0.0f)
          {
               return false;
          }
          
          if(md < 0.0f)
          {
               t = -mn / nn;
          }
          else if(md > dd)
          {
               t = (nd - mn) / nn;
          }
          else
          {
               t = 0.0f;
          }
          return true;
     }

     f32 b_ = dd * mn - nd * md;
     f32 discr = b_*b_ - a_ * c;
     if(discr < 0.0f)
     {
          return false;
     }

     t = (-b_ - sqrtf(discr)) / a_;

     if(t < 0.0f || t > 1.0f)
     {
          return false;
     }

     if(md + t * nd < 0.0f)
     {
          if(nd <= 0.0f)
          {
               return false;
          }
          t = -md / nd;
          return k + 2 * t * (mn + t * nn) <= 0.0f;
     }
     else if(md + t * nd > dd)
     {
          if(nd >= 0.0f)
          {
               return false;
          }
          t = (dd - md) / nd;
          return k + dd - 2 * md + t * (2 * (mn - nd) + t * nn) <= 0.0f;
     }
     
     return true;
}


vec3 Segment::ClosestPoint(vec3 point, f32& t)
{
     vec3 ab = b - a;
     t = dot(point - a, ab) / dot(ab, ab);

     if(t < 0.0f) t = 0.0f;
     if(t > 1.0f) t = 1.0f;

     return a + t * ab;
}

void Plane::Init(vec3 n_, f32 d_)
{
     n = n_;
     d = d_;
}

void Plane::Init(Triangle &triangle)
{
     n = normalize(triangle.n);
     d = dot(triangle.a, n);
}

bool Plane::Intersect(Ray& ray, f32& t)
{
     f32 denom = dot(n, ray.d);
     if (fabsf(denom) > 0.0001f)
     {
          vec3 center = n * d;
          t = dot((center - ray.o), n) / denom;
          if (t >= 0) return true;
     }
     return false;
}

bool Plane::Intersect(Segment& segment, f32& t)
{
     vec3 ab = segment.b - segment.a;
     vec3 dir = normalize(ab);
     f32 len = length(ab);
     
     f32 denom = dot(n, dir);
     if (fabsf(denom) > 0.0001f)
     {
          vec3 center = n * d;
          t = dot((center - segment.a), n) / denom;
          if (t >= 0 && t <= len) return true;
     }
     return false;
}

void Sphere::Init(vec3 c_, f32 r_)
{
     c = c_;
     r = r_;
}


bool Sphere::Intersect(Ray& ray, f32& t)
{
     return ray.Intersect(*this, t);
}

bool Sphere::Intersect(Segment& segment, f32& t)
{
     return segment.Intersect(*this, t);
}

bool Sphere::DynamicIntersect(Plane& plane, vec3 movement, f32& t)
{
     // Compute distance of sphere center to plane
     f32 d = dot(plane.n, c);
     f32 dist = d - plane.d;
     if(fabsf(dist) <= r)
     {
          // The sphere is already overlapping the plane. Set time of
          // intersection to zero
          t = 0.0f;
          return true;
     }
     else
     {
          f32 denom = dot(plane.n, movement);
          if(denom * dist >= 0.0f)
          {
               // No intersection as sphere parallel to or away from plane
               return false;
          }
          else
          {
               // Sphere is moving towards the plane

               f32 radio = dist > 0.0f ? r : -r;
               t = (radio - dist) / denom;
               if(t < 0.0f || t > 1.0f)
               {
                    return false;
               }
               return true;
          }
     }
}

bool Sphere::DynamicIntersect(Triangle& triangle, vec3 movement, f32& u, f32& v, f32& w,
                              f32& t, vec3& n)
{         
     Plane plane;
     plane.Init(triangle);

     if(DynamicIntersect(plane, movement, t))
     {
          
          vec3 q = c + t * movement - r * plane.n;
          if(triangle.PointInside(q, u, v, w))
          {
               n = triangle.n;
               return true;
          }
          else
          {
               Segment segment;
               segment.Init(c, c + movement);
               
               Cylinder cylinders[3];
               cylinders[0].Init(triangle.a, triangle.b, r);
               cylinders[1].Init(triangle.b, triangle.c, r);
               cylinders[2].Init(triangle.c, triangle.a, r);

               
               t = FLT_MAX;
               for(i32 i = 0; i < 3; i++)
               {
                    f32 currentT;
                    if(segment.Intersect(cylinders[i], currentT))
                    {
                         if(currentT < t)
                         {
                              Segment segment;
                              segment.Init(cylinders[i].p, cylinders[i].q);

                              t = currentT;
                              vec3 hitPos = c + movement * t;

                              f32 t0;
                              vec3 closest = segment.ClosestPoint(hitPos, t0);

                              n = normalize(hitPos - closest);                    
                         }
                    }
               }

               if(t != FLT_MAX)
               {
                    // TODO: calculate barycentric
                    return true;
               }
               else
               {
                    Sphere spheres[3];
                    spheres[0].Init(triangle.a, r);
                    spheres[1].Init(triangle.b, r);
                    spheres[2].Init(triangle.c, r);

                    t = FLT_MAX;
                    for(i32 i = 0; i < 3; i++)
                    {
                         f32 currentT;
                         if(segment.Intersect(spheres[i], currentT))
                         {
                              if(currentT < t)
                              {
                                   t = currentT;
                                   vec3 hitPos = c + movement * t;
                                   n = normalize(hitPos - spheres[i].c);
                              }
                         }
                    }
                    return t != FLT_MAX;
               }               
          }
     }
     else
     {
          Segment segment;
          segment.Init(c, c + movement);
               
          Cylinder cylinders[3];
          cylinders[0].Init(triangle.a, triangle.b, r);
          cylinders[1].Init(triangle.b, triangle.c, r);
          cylinders[2].Init(triangle.c, triangle.a, r);

               
          t = FLT_MAX;
          for(i32 i = 0; i < 3; i++)
          {
               f32 currentT;
               if(segment.Intersect(cylinders[i], currentT))
               {
                    if(currentT < t)
                    {
                         Segment segment;
                         segment.Init(cylinders[i].p, cylinders[i].q);

                         t = currentT;
                         vec3 hitPos = c + movement * t;

                         f32 t0;
                         vec3 closest = segment.ClosestPoint(hitPos, t0);

                         n = normalize(hitPos - closest);                    
                    }
               }
          }

          if(t != FLT_MAX)
          {
               // TODO: calculate barycentric
               return true;
          }
          else
          {
               Sphere spheres[3];
               spheres[0].Init(triangle.a, r);
               spheres[1].Init(triangle.b, r);
               spheres[2].Init(triangle.c, r);

               t = FLT_MAX;
               for(i32 i = 0; i < 3; i++)
               {
                    f32 currentT;
                    if(segment.Intersect(spheres[i], currentT))
                    {
                         if(currentT < t)
                         {
                              t = currentT;
                              vec3 hitPos = c + movement * t;
                              n = normalize(hitPos - spheres[i].c);
                         }
                    }
               }
               return t != FLT_MAX;
          }
     }
     
     return false;
}

void Cylinder::Init(vec3 p_, vec3 q_, f32 r_)
{
     p = p_;
     q = q_;
     r = r_;
}

void Triangle::Init(vec3 a_, vec3 b_, vec3 c_)
{
     a = a_;
     b = b_;
     c = c_;
     vec3 ab = b - a;
     vec3 ac = c - a;
     // test if the normal is correct
     n = normalize(cross(ac, ab));
}

bool Triangle::Intersect(Ray& ray, f32& u, f32& v, f32& w, f32& t)
{
     return ray.Intersect(*this, u, v, w, t);
}

bool Triangle::Intersect(Segment& segment, f32& u, f32& v, f32& w, f32& t)
{
     return segment.Intersect(*this, u, v, w, t);
}

bool Triangle::PointInside(vec3 q, f32& u, f32& v, f32& w)
{
     vec3 v0 = b - a;
     vec3 v1 = c - a;
     vec3 v2 = q - a;

     float d00 = dot(v0, v0);
     float d10 = dot(v1, v0);
     float d11 = dot(v1, v1);
     float d20 = dot(v2, v0);
     float d21 = dot(v2, v1);

     float invDenom = 1.0f / ((d00 * d11) - (d10 * d10));

     float y = ((d20 * d11) - (d10 * d21)) * invDenom;
     float z = ((d00 * d21) - (d20 * d10)) * invDenom;
     float x = 1.0f - y - z;

     u = x;
     v = y;
     w = z;

     return x >= 0.0f && x <= 1.0f &&
          y >= 0.0f && y <= 1.0f &&
          z >= 0.0f && z <= 1.0f;
}


void CollisionWorld::LoadFromFile(const char *filepath)
{
     const aiScene *scene = gImporter.ReadFile(filepath, 0);
     i32 totalTriangleCount = 0;
     for(i32 i = 0; i < scene->mNumMeshes; i++)
     {
          aiMesh *mesh = scene->mMeshes[i];
          totalTriangleCount += mesh->mNumFaces;
     }

     ASSERT(totalTriangleCount != 0);
     
     triangles.Init(totalTriangleCount, STATIC_MEMORY);

     for(i32 k = 0; k < scene->mNumMeshes; k++)
     {
          aiMesh *mesh = scene->mMeshes[k];
          for(i32 i = 0; i < mesh->mNumFaces; i++)
          {
               aiFace *face = mesh->mFaces + i;
               ASSERT(face->mNumIndices == 3); // the mesh needs to be triagulated

               auto v0 = mesh->mVertices[face->mIndices[0]];
               auto v1 = mesh->mVertices[face->mIndices[1]];
               auto v2 = mesh->mVertices[face->mIndices[2]];

               vec3 a = vec3(v0.x, v0.y, v0.z*-1.0f);
               vec3 b = vec3(v1.x, v1.y, v1.z*-1.0f);                    
               vec3 c = vec3(v2.x, v2.y, v2.z*-1.0f);
               
               Triangle triangle;
               triangle.Init(a, b, c);

               triangles.Push(triangle);
          }
     }
}

bool CollisionWorld::Intersect(Segment& segment, f32& t, vec3& n)
{
     t = FLT_MAX;
     for(i32 i = 0; i < triangles.size; ++i)
     {
          f32 u, v, w, currentT;
          if(triangles[i].Intersect(segment, u, v, w, currentT))
          {
               if(currentT < t)
               {
                    t = currentT;
                    n = triangles[i].n;
               }
          }
     }
     return t != FLT_MAX;
}

bool CollisionWorld::Intersect(Ray& ray, f32& t, vec3& n)
{
     t = FLT_MAX;
     for(i32 i = 0; i < triangles.size; ++i)
     {
          f32 u, v, w, currentT;
          if(triangles[i].Intersect(ray, u, v, w, currentT))
          {
               if(currentT < t)
               {
                    t = currentT;
                    n = triangles[i].n;
               }
          }
     }
     return t != FLT_MAX;
}

static void SortCollisionRersult(Array<CollisionData>& arr)
{
     for(i32 i = 1; i < arr.size; ++i)
     {
          auto key = arr[i];
          i32 j = i - 1;

          while(j >= 0 && arr[j].t > key.t)
          {
               arr[j + 1] = arr[j];
               j = j - 1;
          }
          arr[j + 1] = key;
     }
}

bool CollisionWorld::DynamicIntersect(Sphere& sphere, vec3 movement,
                                      Array<CollisionData>& collisionData)
{    
     for(i32 i = 0; i < triangles.size; ++i)
     {
          f32 u, v, w, t;
          vec3 n;
          if(sphere.DynamicIntersect(triangles[i], movement, u, v, w, t, n))
          {
               if(collisionData.size < MAX_COLLISION_COUNT)
               {
                    CollisionData cd;
                    cd.n = n;
                    cd.t = t;
                    cd.u = u;
                    cd.v = v;
                    cd.w = w;
                    collisionData.Push(cd);
               }
          }
     }

     if(collisionData.size > 1)
     {
          for(i32 i = 0; i < collisionData.size; ++i)
          {
               bool collisionAdded = false;
               for(i32 j = 0; j < collisionData.size; ++j)
               {
                    if(i != j)
                    {
                         vec3 n0 = collisionData[i].n;
                         vec3 n1 = collisionData[j].n;
                         f32 proj = dot(n0, n1); 
                         if(proj < 0.0f && proj > -0.999f)
                         {
                              CollisionData collision = {};
                              collision.n = normalize(n0 + n1);
                              collisionData.Push(collision);
                              collisionAdded = true;
                              break;
                         }     
                    }
               }
               if(collisionAdded)
               {
                    break;
               }
          }
     }
          
     SortCollisionRersult(collisionData);
     return collisionData.size > 0;
}
