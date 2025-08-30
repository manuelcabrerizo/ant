#include "batch_renderer_d3d11.h"

#include <graphics_manager.h>
#include <memory_manager.h>
#include <d3d11.h>
#include <math/matrix4.h>

VertexQuad vertices[] =
{
    VertexQuad{ Vector3(-0.5, -0.5, 0), Vector2(0, 0) },
    VertexQuad{ Vector3(-0.5,  0.5, 0), Vector2(0, 1) },
    VertexQuad{ Vector3(0.5,  0.5, 0), Vector2(1, 1) },
    VertexQuad{ Vector3(0.5,  0.5, 0), Vector2(1, 1) },
    VertexQuad{ Vector3(0.5, -0.5, 0), Vector2(1, 0) },
    VertexQuad{ Vector3(-0.5, -0.5, 0), Vector2(0, 0) }
};

void BatchRendererD3D11::Init(ID3D11Device* device, ID3D11DeviceContext* deviceContext,
    VertexShader* vertShader, FragmentShader *fragShader, Texture* texture)
{
    BatchRenderer::Init(vertShader, fragShader, texture);
    this->deviceContext = deviceContext;

    D3D11_BUFFER_DESC vertexDesc;
    ZeroMemory(&vertexDesc, sizeof(vertexDesc));
    vertexDesc.Usage = D3D11_USAGE_DYNAMIC;
    vertexDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vertexDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    vertexDesc.ByteWidth = sizeof(VertexQuad) * cpuBuffer.GetCapacity();
    if (FAILED(device->CreateBuffer(&vertexDesc, 0, &gpuBuffer)))
    {
        ASSERT(!"Error create batch renderer GPU buffer.");
    }
}

void BatchRendererD3D11::Terminate()
{
    cpuBuffer.Clear();
    gpuBuffer->Release();
}

void BatchRendererD3D11::Present()
{
    D3D11_MAPPED_SUBRESOURCE bufferData;
    ZeroMemory(&bufferData, sizeof(bufferData));
    deviceContext->Map(gpuBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &bufferData);
    ASSERT(cpuBuffer.size <= cpuBuffer.GetCapacity());
    memcpy(bufferData.pData, cpuBuffer.data, sizeof(VertexQuad) * cpuBuffer.size);
    deviceContext->Unmap(gpuBuffer, 0);

    BatchRenderer::BindResources();

    u32 stride = sizeof(VertexQuad);
    u32 offset = 0;
    deviceContext->IASetVertexBuffers(0, 1, &gpuBuffer, &stride, &offset);
    ASSERT(cpuBuffer.size % 6 == 0);
    deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    deviceContext->Draw(cpuBuffer.size, 0);
    cpuBuffer.size = 0;
}

void BatchRendererD3D11::DrawQuad(const Vector3& position, const Vector3& scale, 
    const Quaternion& rotation, const Vector3& color)
{    
    if (cpuBuffer.size + 6 > cpuBuffer.GetCapacity())
    {
        Present();
    }

    ASSERT(cpuBuffer.size + 6 <= cpuBuffer.GetCapacity());

    Matrix4 rotMat = rotation.ToMatrix4();

    VertexQuad* vertex = cpuBuffer.data + cpuBuffer.size;
    for (int i = 0; i < 6; i++)
    {
        vertex->pos.x = vertices[i].pos.x * scale.x;
        vertex->pos.y = vertices[i].pos.y * scale.y;
        vertex->pos.z = 0;
        vertex->pos = Matrix4::TransformPoint(rotMat, vertex->pos);
        vertex->pos += position;
        vertex->uv = vertices[i].uv;
        vertex->col = color;
        vertex++;
    }

    cpuBuffer.size += 6;
}