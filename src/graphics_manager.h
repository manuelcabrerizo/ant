#pragma once

#include "platform.h"
#include "math/vector3.h"

#define WINDOW_WIDTH 1280
#define WINDOW_HEIGHT 720

enum class GraphicsManagerType
{
     D3D11,
     D3D12,
     OPENGL,
     METAL,
     VULKAN
};

enum UniformBufferBind
{
     BIND_TO_VS = (1 << 0),
     BIND_TO_PS = (1 << 1),
     BIND_TO_GS = (1 << 2)
};

struct VertexBuffer {};
struct IndexBuffer {};
struct UniformBuffer {};
struct FrameBuffer {};
struct VertexShader {};
struct FragmentShader {};
struct Texture {};

class GraphicsManager
{
private:
     static GraphicsManager *instance;
public:
    virtual ~GraphicsManager() {}

     static void Init(void *osWindow, i32 width, i32 height, GraphicsManagerType type, i32 stackNum);
     static void Terminate();
     static GraphicsManager *Get();

     virtual void Initialize(void *osWindow, i32 width, i32 height, i32 stackNum) = 0;
     virtual void Shutdown() = 0;

     virtual void OnResize(i32 width, i32 height) = 0;
     
     virtual void BeginFrame(f32 r, f32 g, f32 b) = 0;
     virtual void EndFrame(i32 vsync) = 0;

     virtual void Draw(u32 verticesCount) = 0;
     virtual void DrawIndexed(u32 indexCount) = 0;

     virtual void SetRasterizerStateCullBack() = 0;
     virtual void SetRasterizerStateCullFront() = 0;
     virtual void SetRasterizerStateCullNone() = 0;
     virtual void SetRasterizerStateWireframe() = 0;
     
     virtual VertexBuffer *VertexBufferAlloc(void *vertices, u32 count, u32 stride) = 0;
     virtual void VertexBufferFree(VertexBuffer *vertexBuffer) = 0;
     virtual void VertexBufferBind(VertexBuffer *vertexBuffer) = 0;

     virtual IndexBuffer *IndexBufferAlloc(u32 *indices, u32 count) = 0;
     virtual void IndexBufferFree(IndexBuffer *indexBuffer) = 0;
     virtual void IndexBufferBind(IndexBuffer *indexBuffer) = 0;

     virtual UniformBuffer *UniformBufferAlloc(u32 bindTo, void *data, u32 dataSize, u32 slot) = 0;
     virtual void UniformBufferFree(UniformBuffer *uniformBuffer) = 0;
     virtual void UniformBufferBind(UniformBuffer *uniformBuffer) = 0;
     virtual void UniformBufferUpdate(UniformBuffer *uniformBuffer, void *data) = 0;

     virtual VertexShader *VertexShaderAlloc(File file) = 0;
     virtual void VertexShaderFree(VertexShader* shader) = 0;
     virtual void VertexShaderBind(VertexShader* shader) = 0;

     virtual FragmentShader* FragmentShaderAlloc(File file) = 0;
     virtual void FragmentShaderFree(FragmentShader* shader) = 0;
     virtual void FragmentShaderBind(FragmentShader* shader) = 0;

     virtual Texture *TextureAlloc(const char *filepath) = 0;
     virtual void TextureFree(Texture *texture) = 0;
     virtual void TextureBind(Texture *texture, i32 slot) = 0;
     virtual i32 TextureWidth(Texture *texture) = 0;
     virtual i32 TextureHeight(Texture *texture) = 0;

     virtual void DebugInit() = 0;
     virtual void DebugTerminate() = 0;
     virtual void DebugPresent() = 0;
     virtual void DebugDrawLine(const Vector3& a, const Vector3& b, const Vector3& color) = 0;
     virtual void DebugDrawSphere(const Vector3& c, f32 r, i32 hSlice, i32 vSlice, const Vector3& color) = 0;
     virtual void DebugDrawCube(const Vector3& c, const Vector3& hExtend, const Vector3& color) = 0;
};
