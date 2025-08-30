#pragma once

#include <batch_renderer.h>

#include <common.h>
#include <math/vector3.h>
#include <math/vector2.h>
#include <containers.h>

struct ID3D11Buffer;
struct ID3D11Device;
struct ID3D11DeviceContext;

struct VertexQuad
{
    Vector3 pos;
    Vector2 uv;
    Vector3 col;
};

class BatchRendererD3D11 : public BatchRenderer
{
private:
    ID3D11DeviceContext* deviceContext = nullptr;
    ID3D11Buffer* gpuBuffer = nullptr;
    StaticArray<VertexQuad, 12000> cpuBuffer;
public:
    void Init(ID3D11Device* device, ID3D11DeviceContext* deviceContext,
        VertexShader *vertShader, FragmentShader *fragShader, Texture *texture);
    void Terminate() override;
    void Present() override;
    void DrawQuad(const Vector3& position, const Vector3& scale,
        const Quaternion& rotation, const Vector3& color) override;
};