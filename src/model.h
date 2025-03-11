#pragma once

struct Vertex
{
     vec3 pos;
     vec3 nor;
     vec2 uvs;
};

class Model
{
private:
     VertexBuffer vertexBuffer;
     IndexBuffer indexBuffer;
     u32 verticesCount;
     u32 indicesCount;
public:
     void Init(const char *filepath);
     void Init(Vertex *vertices, u32 verticesCount,
               u32 *indices, u32 indicesCount);
     void Terminate();
     void Draw();
};
