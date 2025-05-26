#pragma once

#include <common.h>
#include <math/vector3.h>

struct Shader;

struct ID3D11Buffer;
struct ID3D11Device;
struct ID3D11DeviceContext;

struct VertexLine
{
     Vector3 pos;
     Vector3 col;
};

class DebugRendererD3D11
{
private:
     ID3D11Buffer *gpuBuffer;
     VertexLine   *cpuBuffer;
     u64 bufferSize;
     u32 bufferUsed;

     Shader *shader;
public:
     void Init(ID3D11Device *device);
     void Terminate();
     void Present(ID3D11DeviceContext *deviceContext);

     void DrawLine(ID3D11DeviceContext *deviceContext, Vector3& a, Vector3 &b);
};
