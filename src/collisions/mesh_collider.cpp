#include "mesh_collider.h"

#include "collision_utils.h"
#include <collision.h>

#include "aabb.h"
#include "obb.h"
#include "sphere.h"
#include "capsule.h"

#include <utils.h>
#include <assimp/postprocess.h>

void MeshCollider::InitFromFile(const char* filepath)
{
    const aiScene* scene = Utils::importer.ReadFile(filepath, aiProcess_MakeLeftHanded | aiProcess_FlipWindingOrder);
    i32 totalTriangleCount = 0;
    for (i32 i = 0; i < scene->mNumMeshes; i++)
    {
        aiMesh* mesh = scene->mMeshes[i];
        totalTriangleCount += mesh->mNumFaces;
    }

    ASSERT(totalTriangleCount != 0);

    triangles.Init(totalTriangleCount, STATIC_MEMORY);

    for (i32 k = 0; k < scene->mNumMeshes; k++)
    {
        aiMesh* mesh = scene->mMeshes[k];
        for (i32 i = 0; i < mesh->mNumFaces; i++)
        {
            aiFace* face = mesh->mFaces + i;
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

bool MeshCollider::Intersect(const AABB& aabb, Array<CollisionData>* collisionData) const
{
    // TODO: implement this function
    return false;
}

bool MeshCollider::Intersect(const OBB& obb, Array<CollisionData>* collisionData) const
{
    bool isIntersecting = false;
    for (int i = 0; i < triangles.size; ++i)
    {
        isIntersecting |= obb.Intersect(triangles[i], collisionData);
    }
    return isIntersecting;
}

bool MeshCollider::Intersect(const Sphere& sphere, Array<CollisionData>* collisionData) const
{
    bool isIntersecting = false;
    for (int i = 0; i < triangles.size; ++i)
    {
        isIntersecting |= sphere.Intersect(triangles[i], collisionData);
    }
    return isIntersecting;
}

bool MeshCollider::Intersect(const Capsule& capsule, Array<CollisionData>* collisionData) const
{
    bool isIntersecting = false;
    for (int i = 0; i < triangles.size; ++i)
    {
        isIntersecting |= capsule.Intersect(triangles[i], collisionData);

    }
    return isIntersecting;
}

const Array<Triangle>& MeshCollider::GetTriangles() const
{
    return triangles;
}

