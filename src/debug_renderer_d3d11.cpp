#include <debug_renderer_d3d11.h>

#include <platform.h>
#include <memory_manager.h>
#include <graphics_manager.h>

#include <d3d11_1.h>

#include <stdio.h>

void DebugRendererD3D11::Init(ID3D11Device *device)
{
     bufferUsed = 0;
     bufferSize = 10000;

     cpuBuffer = (VertexLine *)MemoryManager::Get()->Alloc(sizeof(VertexLine) * bufferSize, STATIC_MEMORY);

     D3D11_BUFFER_DESC vertexDesc;
     ZeroMemory(&vertexDesc, sizeof(vertexDesc));
     vertexDesc.Usage = D3D11_USAGE_DYNAMIC;
     vertexDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
     vertexDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
     vertexDesc.ByteWidth = sizeof(VertexLine) * bufferSize;
     if(FAILED(device->CreateBuffer(&vertexDesc, 0, &gpuBuffer)))
     {
          ASSERT(!"Error create line renderer GPU buffer.");
     }
     
     Frame frame = MemoryManager::Get()->GetFrame(SCRATCH_MEMORY);
     // Load the shader
     File vertFile = PlatformReadFile("data/shaders/line_vert.hlsl", SCRATCH_MEMORY);
     File fragFile = PlatformReadFile("data/shaders/line_frag.hlsl", SCRATCH_MEMORY);
     vertexShader = GraphicsManager::Get()->VertexShaderAlloc(vertFile);
     fragmentShader = GraphicsManager::Get()->FragmentShaderAlloc(fragFile);

     MemoryManager::Get()->ReleaseFrame(frame);

     printf("Debug Renderer Init!\n");
     
}

void DebugRendererD3D11::Terminate()
{
     gpuBuffer->Release();
     GraphicsManager::Get()->VertexShaderFree(vertexShader);
     GraphicsManager::Get()->FragmentShaderFree(fragmentShader);
     printf("Debug Renderer Terminate!\n");
}

void DebugRendererD3D11::Present(ID3D11DeviceContext *deviceContext)
{
     D3D11_MAPPED_SUBRESOURCE bufferData;
     ZeroMemory(&bufferData, sizeof(bufferData));
     deviceContext->Map(gpuBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &bufferData);
     ASSERT(bufferUsed <= bufferSize);
     memcpy(bufferData.pData, cpuBuffer, sizeof(VertexLine)*bufferUsed);
     deviceContext->Unmap(gpuBuffer, 0);

     GraphicsManager::Get()->VertexShaderBind(vertexShader);
     GraphicsManager::Get()->FragmentShaderBind(fragmentShader);
     
     u32 stride = sizeof(VertexLine);
     u32 offset = 0;
     deviceContext->IASetVertexBuffers(0, 1, &gpuBuffer, &stride, &offset);
     ASSERT(bufferUsed % 2 == 0);
     deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
     deviceContext->Draw(bufferUsed, 0);
     bufferUsed = 0;
}


void DebugRendererD3D11::DrawLine(ID3D11DeviceContext *deviceContext, const Vector3& a, const Vector3 &b, const Vector3& color)
{
     VertexLine line[2] = {};
     line[0].pos = a;
     line[0].col = color;
     line[1].pos = b;
     line[1].col = color;

     if(bufferUsed + 2 > bufferSize)
     {
          Present(deviceContext);
     }
     
     ASSERT(bufferUsed + 2 <= bufferSize);
     VertexLine *vertex = cpuBuffer + bufferUsed;
     memcpy(vertex, line, sizeof(VertexLine)*2);
     bufferUsed += 2;
}
