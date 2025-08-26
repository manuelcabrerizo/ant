#include "mesh_collider.h"

#include "collision_utils.h"
#include <collision.h>

#include "aabb.h"
#include "obb.h"
#include "sphere.h"
#include "capsule.h"

#include <utils.h>
#include <assimp/postprocess.h>

#include <windows.h>

void MeshCollider::InitFromFile(const char* filepath)
{
    const aiScene* scene = Utils::importer.ReadFile(filepath, aiProcess_MakeLeftHanded | aiProcess_FlipWindingOrder | aiProcess_Triangulate);
    i32 totalTriangleCount = 0;
    for (i32 i = 0; i < scene->mNumMeshes; i++)
    {
        aiMesh* mesh = scene->mMeshes[i];
        totalTriangleCount += mesh->mNumFaces;
    }

    ASSERT(totalTriangleCount != 0);

    triangles.Init(totalTriangleCount, FRAME_MEMORY);
    boundingBoxes.Init(totalTriangleCount, FRAME_MEMORY);

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

    // Compute the triangles AABBs
    for (int i = 0; i < triangles.size; ++i)
    {
        Frame frame = MemoryManager::Get()->GetFrame(SCRATCH_MEMORY);

        Array<Vector3> points;

        points.Init(3, SCRATCH_MEMORY);
        points.Push(triangles[i].a);
        points.Push(triangles[i].b);
        points.Push(triangles[i].c);

        AABB aabb;
        aabb.Init(points);
        boundingBoxes.Push(aabb);

        MemoryManager::Get()->ReleaseFrame(frame);
    }

    // Create the BVH tree
    void* rootBuffer = MemoryManager::Get()->Alloc(sizeof(BVHNode), FRAME_MEMORY);
    root = new (rootBuffer) BVHNode(nullptr, boundingBoxes[0], &triangles[0]);
    for (int i = 1; i < boundingBoxes.size; i++)
    {
        root->Insert(boundingBoxes[i], &triangles[i]);
    }
}

bool MeshCollider::Intersect(const AABB& aabb, Array<CollisionData>* collisionData) const
{
    OBB obb;
    obb.Init(aabb);
    bool isIntersecting = false;
    for (int i = 0; i < triangles.size; ++i)
    {
        isIntersecting |= obb.Intersect(triangles[i], collisionData);
    }
    return isIntersecting;
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
#if 0
    // 10 FPS on debug 70 FPS on release
    bool isIntersecting = false;
    for (int i = 0; i < triangles.size; ++i)
    {
        isIntersecting |= capsule.Intersect(triangles[i], collisionData);
    }
    return isIntersecting;
#else 
    // 140 FPS on debug 700 FPS on release
    return root->Intersect(capsule, collisionData);
#endif
}

const Array<Triangle>& MeshCollider::GetTriangles() const
{
    return triangles;
}

void MeshCollider::DebugDraw(const Vector3& color) const
{
    for (int i = 0; i < boundingBoxes.size; i++)
    {
        boundingBoxes[i].DebugDraw(color);
    }
}


