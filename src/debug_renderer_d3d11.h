#pragma once

#include <common.h>
#include <math/vector3.h>

struct ID3D11Buffer;
struct ID3D11Device;
struct ID3D11DeviceContext;

struct VertexShader;
struct FragmentShader;

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

     VertexShader* vertexShader;
     FragmentShader* fragmentShader;
public:
     void Init(ID3D11Device *device);
     void Terminate();
     void Present(ID3D11DeviceContext *deviceContext);

     void DrawLine(ID3D11DeviceContext *deviceContext, const Vector3& a, const Vector3 &b, const Vector3& color);
};
