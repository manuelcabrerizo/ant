#pragma once

#include <containers.h>

#include <math/vector3.h>
#include <math/vector2.h>
#include <math/matrix4.h>

#include "graphics_manager.h"

class Material;

// TODO: separate static from animated vertex
#define MAX_BONE_INFLUENCE 4
struct Vertex
{
     Vector3 pos;
     Vector3 nor;
     Vector2 uvs;
     i32 boneId[MAX_BONE_INFLUENCE];
     f32 weights[MAX_BONE_INFLUENCE];
};

class Mesh
{
private:
     VertexBuffer *vertexBuffer;
     IndexBuffer *indexBuffer;
     u32 verticesCount;
     u32 indicesCount;
     Material* material;
public:
     void Init(Vertex *vertices, u32 verticesCount,
               u32 *indices, u32 indicesCount);
     void Terminate();
     void Draw();

     void SetMaterial(Material* material);
};

struct BoneInfo
{
     i32 id = -1;
     Matrix4 offset = Matrix4(1.0f);
};

class Model
{
private:
     Array<Mesh> meshes;
     HashMap<BoneInfo> bonesInfo;
     i32 boneCounter = 0;
     int memoryType;
public:
     void Init(const char *filepath, i32 memoryType);
     void Terminate();
     void Draw();
     HashMap<BoneInfo>& GetBonesInfo();
     i32 GetBoneCount() 
     {
          return boneCounter;
     }

     i32 GetMeshCount()
     {
         return meshes.size;
     }

     int GetMemoryType()
     {
         return memoryType;
     }

     void SetDefaultMaterial(Material* material);
     void SetMaterialAtMeshIndex(i32 index, Material* material);
};
