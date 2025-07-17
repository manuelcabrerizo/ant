#include <collision.h>

#include <math/algebra.h>
#include <float.h>

#include <utils.h>
#include <assimp/postprocess.h>

void CollisionWorld::LoadFromFile(const char *filepath)
{
     const aiScene *scene = Utils::importer.ReadFile(filepath, aiProcess_MakeLeftHanded | aiProcess_FlipWindingOrder);
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
          f32 currentT;
          if(triangles[i].Intersect(segment, currentT))
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
          f32 currentT;
          if(triangles[i].Intersect(ray, currentT))
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

bool CollisionWorld::Intersect(const Capsule& capsule, Array<CollisionData>& collisionData)
{
    for (i32 i = 0; i < triangles.size; ++i)
    {
        f32 penetration;
        Vector3 n;
        if (capsule.Intersect(triangles[i], n, penetration))
        {
            if (collisionData.size < MAX_COLLISION_COUNT)
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
         f32 t = 0.0f;
          Vector3 n;
          if(sphere.DynamicIntersect(triangles[i], movement, t, n))
          {
               if(collisionData.size < MAX_COLLISION_COUNT)
               {
                    CollisionData cd;
                    cd.n = n;
                    cd.t = t;
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