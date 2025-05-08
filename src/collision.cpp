
void Ray::Init(Vector3 o, Vector3 d)
{
     this->o = o;
     this->d = d;
}

bool Ray::Intersect(Triangle& triangle, f32& u, f32& v, f32& w, f32& t)
{
     Plane plane;
     plane.Init(triangle);
     
     if(plane.Intersect(*this, t))
     {
          Vector3 hitPos = o + d * t;
          return triangle.PointInside(hitPos, u, v, w);
     }
     return false;
}

bool Ray::Intersect(Sphere& sphere, f32& t)
{
     Vector3 m = o - sphere.c;
     f32 b = m.Dot(d);
     f32 c = m.Dot(m) - sphere.r * sphere.r;

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


void Segment::Init(Vector3 a, Vector3 b)
{
     this->a = a;
     this->b = b;
}

bool Segment::Intersect(Triangle& triangle, f32& u, f32& v, f32& w, f32& t)
{
     Plane plane;
     plane.Init(triangle);

     if(plane.Intersect(*this, t))
     {
          Vector3 hitPos = a + (b - a).Normalized() * t;
          return triangle.PointInside(hitPos, u, v, w);
     }
     return false;
}

bool Segment::Intersect(Sphere& sphere, f32& t)
{
     Vector3 ab = b - a;
     Vector3 o = a;
     Vector3 d = ab.Normalized();

     Vector3 m = o - sphere.c;
     f32 b_ = m.Dot(d);
     f32 c = m.Dot(m) - sphere.r * sphere.r;

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

     if(t*t > ab.Dot(ab))
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
     Vector3 d = cylinder.q - cylinder.p, m = a - cylinder.p, n = b - a;
     f32 md = m.Dot(d);
     f32 nd = n.Dot(d);
     f32 dd = d.Dot(d);

     if(md < 0.0f && md + nd < 0.0f)
     {
          return false;
     }
     if(md > dd && md + nd > dd)
     {
          return false;
     }

     f32 nn = n.Dot(n);
     f32 mn = m.Dot(n);
     f32 a_ = dd * nn - nd * nd;
     f32 k = m.Dot(m) - cylinder.r * cylinder.r;
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


Vector3 Segment::ClosestPoint(Vector3 point, f32& t)
{
     Vector3 ab = b - a;
     t = (point - a).Dot(ab) / ab.Dot(ab);

     if(t < 0.0f) t = 0.0f;
     if(t > 1.0f) t = 1.0f;

     return a + ab * t;
}

void Plane::Init(Vector3 n, f32 d)
{
     this->n = n;
     this->d = d;
}

void Plane::Init(Triangle &triangle)
{
     n = triangle.n.Normalized();
     d = triangle.a.Dot(n);
}

bool Plane::Intersect(Ray& ray, f32& t)
{
     f32 denom = n.Dot(ray.d);
     if (fabsf(denom) > 0.0001f)
     {
          Vector3 center = n * d;
          t = (center - ray.o).Dot(n) / denom;
          if (t >= 0) return true;
     }
     return false;
}

bool Plane::Intersect(Segment& segment, f32& t)
{
     Vector3 ab = segment.b - segment.a;
     Vector3 dir = ab.Normalized();
     f32 len = ab.Magnitude();
     
     f32 denom = n.Dot(dir);
     if (fabsf(denom) > 0.0001f)
     {
          Vector3 center = n * d;
          t = (center - segment.a).Dot(n) / denom;
          if (t >= 0 && t <= len) return true;
     }
     return false;
}


Vector3 Plane::ClosestPoint(Vector3 q)
{
     f32 t = (n.Dot(q) - d) / n.Dot(n);
     return q - n * t;
}

void Sphere::Init(Vector3 c, f32 r)
{
     this->c = c;
     this->r = r;
}

bool Sphere::Intersect(Ray& ray, f32& t)
{
     return ray.Intersect(*this, t);
}

bool Sphere::Intersect(Segment& segment, f32& t)
{
     return segment.Intersect(*this, t);
}

bool Sphere::Intersect(Triangle& triangle, Vector3& n, f32& penetration)
{
     Vector3 closest = triangle.ClosestPoint(c);

     //GraphicsManager::Get()->DebugDrawSphere(closest, 0.02f, 4, 6);

     Vector3 toSphere = c - closest;
     
     f32 lenSq = toSphere.Dot(toSphere);

     n = toSphere.Normalized();

     //GraphicsManager::Get()->DebugDrawLine(closest, closest + n);
     
     penetration = r - sqrtf(lenSq);
     if(lenSq <= r*r)
     {
          i32 StopHere = 0;
     }
     
     return lenSq <= r*r;
}

bool Sphere::DynamicIntersect(Plane& plane, Vector3 movement, f32& t)
{
     // Compute distance of sphere center to plane
     f32 d = plane.n.Dot(c);
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
          f32 denom = plane.n.Dot(movement);
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

bool Sphere::DynamicIntersect(Triangle& triangle, Vector3 movement, f32& u, f32& v, f32& w,
                              f32& t, Vector3& n)
{         
     Plane plane;
     plane.Init(triangle);

     if(DynamicIntersect(plane, movement, t))
     {
          
          Vector3 q = c + movement * t - plane.n * r;
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
                              Vector3 hitPos = c + movement * t;

                              f32 t0;
                              Vector3 closest = segment.ClosestPoint(hitPos, t0);

                              n = (hitPos - closest).Normalized();                    
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
                                   Vector3 hitPos = c + movement * t;
                                   n = (hitPos - spheres[i].c).Normalized();
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
                         Vector3 hitPos = c + movement * t;

                         f32 t0;
                         Vector3 closest = segment.ClosestPoint(hitPos, t0);

                         n = (hitPos - closest).Normalized();                    
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
                              Vector3 hitPos = c + movement * t;
                              n = (hitPos - spheres[i].c).Normalized();
                         }
                    }
               }
               return t != FLT_MAX;
          }
     }
     
     return false;
}

void Cylinder::Init(Vector3 p, Vector3 q, f32 r)
{
     this->p = p;
     this->q = q;
     this->r = r;
}

void Triangle::Init(Vector3 a, Vector3 b, Vector3 c)
{
     this->a = a;
     this->b = b;
     this->c = c;
     Vector3 ab = b - a;
     Vector3 ac = c - a;
     // test if the normal is correct
     n = ac.Cross(ab).Normalized();
}

bool Triangle::Intersect(Ray& ray, f32& u, f32& v, f32& w, f32& t)
{
     return ray.Intersect(*this, u, v, w, t);
}

bool Triangle::Intersect(Segment& segment, f32& u, f32& v, f32& w, f32& t)
{
     return segment.Intersect(*this, u, v, w, t);
}

bool Triangle::PointInside(Vector3 q, f32& u, f32& v, f32& w)
{
     Vector3 v0 = b - a;
     Vector3 v1 = c - a;
     Vector3 v2 = q - a;

     float d00 = v0.Dot(v0);
     float d10 = v1.Dot(v0);
     float d11 = v1.Dot(v1);
     float d20 = v2.Dot(v0);
     float d21 = v2.Dot(v1);

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

Vector3 Triangle::ClosestPoint(Vector3 q)
{
     Plane plane;
     plane.Init(*this);

     Vector3 pointInPlane = plane.ClosestPoint(q);
     f32 u, v, w;
     if(PointInside(pointInPlane, u, v, w))
     {
          return pointInPlane;
     }

     Segment edges[3];
     edges[0].Init(a, b);
     edges[1].Init(b, c);
     edges[2].Init(c, a);

     Vector3 result;
     
     f32 lenSq = FLT_MAX;
     for(i32 i = 0; i < 3; ++i)
     {
          f32 currentT;
          Vector3 closest = edges[i].ClosestPoint(q, currentT);

          f32 currentLenSq = (closest - q).MagnitudeSq(); 
          if(currentLenSq < lenSq)
          {
               lenSq = currentLenSq;
               result = closest;
          }
     }
     
     return result;
}

// TODO: fix gImporter architecture
void CollisionWorld::LoadFromFile(const char *filepath)
{
     const aiScene *scene = gImporter.ReadFile(filepath, aiProcess_MakeLeftHanded | aiProcess_FlipWindingOrder);
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

               Vector3 a = Vector3(v0.x, v0.y, v0.z);
               Vector3 b = Vector3(v1.x, v1.y, v1.z);                    
               Vector3 c = Vector3(v2.x, v2.y, v2.z);
               
               Triangle triangle;
               triangle.Init(a, b, c);
               triangles.Push(triangle);
          }
     }
}

bool CollisionWorld::Intersect(Segment& segment, f32& t, Vector3& n)
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

bool CollisionWorld::Intersect(Ray& ray, f32& t, Vector3& n)
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

static void SortCollisionByTime(Array<CollisionData>& arr)
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

static void SortCollisionByPenetration(Array<CollisionData>& arr)
{
     for(i32 i = 1; i < arr.size; ++i)
     {
          auto key = arr[i];
          i32 j = i - 1;

          while(j >= 0 && arr[j].penetration < key.penetration)
          {
               arr[j + 1] = arr[j];
               j = j - 1;
          }
          arr[j + 1] = key;
     }
}

bool CollisionWorld::Intersect(Sphere& sphere, Array<CollisionData>& collisionData)
{
     for(i32 i = 0; i < triangles.size; ++i)
     {
          f32 penetration;
          Vector3 n;
          if(sphere.Intersect(triangles[i], n, penetration))
          {
               if(collisionData.size < MAX_COLLISION_COUNT)
               {
                    CollisionData cd;
                    cd.n = n;
                    cd.penetration = penetration;
                    collisionData.Push(cd);
               }
          }
     }
     SortCollisionByPenetration(collisionData);
     return collisionData.size > 0;
}

bool CollisionWorld::DynamicIntersect(Sphere& sphere, Vector3 movement,
                                      Array<CollisionData>& collisionData)
{    
     for(i32 i = 0; i < triangles.size; ++i)
     {
          f32 u, v, w, t;
          Vector3 n;
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
                         Vector3 n0 = collisionData[i].n;
                         Vector3 n1 = collisionData[j].n;
                         f32 proj = n0.Dot(n1); 
                         if(proj < 0.0f && proj > -0.999f)
                         {
                              CollisionData collision = {};
                              collision.n = (n0 + n1).Normalized();
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
          
     SortCollisionByTime(collisionData);
     return collisionData.size > 0;
}
