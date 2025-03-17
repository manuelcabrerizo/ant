
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


void Plane::Init(vec3 n_, f32 d_)
{
     n = n_;
     d = d_;
}

void Plane::Init(Triangle &triangle)
{
     n = triangle.n;
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

bool Triangle::Intersect(Sphere& sphere, f32& u, f32& v, f32& w, f32& t)
{
     return false;
}

bool Triangle::Intersect(Capsule& capsule, f32& u, f32& v, f32& w, f32& t)
{
     return false;
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
