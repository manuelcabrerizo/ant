#pragma once

#include <d3d11_1.h>
#include <d3dcompiler.h>

#include <containers.h>
#include <debug_renderer_d3d11.h>

#include <graphics_manager.h>
#include <batch_renderer_d3d11.h>

struct VertexBufferD3D11 : public VertexBuffer
{
     ID3D11Buffer *buffer;
     u32 verticesCount;
     u32 stride;
     u32 offset;
};

struct IndexBufferD3D11 : public IndexBuffer
{
     ID3D11Buffer *buffer;
     u32 indicesCount;
     DXGI_FORMAT format;
};

struct UniformBufferD3D11 : UniformBuffer
{
     ID3D11Buffer *buffer;
     u32 slot;
     u32 bindTo;
     u32 dataSize;
};

struct FrameBufferD3D11 : FrameBuffer
{
     u32 x, y, w, h;
     u32 msaa = 1;
     u32 msaaQuality;
     DXGI_FORMAT format;

     ID3D11Texture2D *texture;
     ID3D11RenderTargetView *renderTargetView;
     ID3D11DepthStencilView *depthStencilView;

     ID3D11Texture2D *resolveTexture;
     ID3D11ShaderResourceView *shaderResourceView;
};

struct VertexShaderD3D11 : VertexShader
{
     ID3D11InputLayout *layout;
     ID3D11VertexShader *vertexShader;
};

struct FragmentShaderD3D11 : FragmentShader
{
    ID3D11PixelShader* pixelShader;
};

struct TextureD3D11 : Texture
{
     i32 w, h;
     ID3D11Texture2D *texture;
     ID3D11ShaderResourceView *shaderResourceView;
};

const int LIGHTS_UBO_BIND_INDEX = 4;
const int MAX_POINT_LIGHTS = 8;

struct LightsUbo
{
    DirectionalLight directionalLight;
    PointLight pointLights[MAX_POINT_LIGHTS];
    int pointLightsCount;
    Vector3 viewPos;
};

class GraphicsManagerD3D11 : public GraphicsManager
{
private:     
     HWND *window;
     i32 windowWidth;
     i32 windowHeight;
     DXGI_SWAP_CHAIN_DESC sd;
     u32 msaaQuality4x;
     
     ID3D11Device *device;
     ID3D11Device1 *device1;
     ID3D11DeviceContext *deviceContext;
     ID3D11DeviceContext1 *deviceContext1;
     IDXGISwapChain *swapChain;
     IDXGISwapChain1 *swapChain1;

     ID3D11RenderTargetView *renderTargetView;
     ID3D11DepthStencilView *depthStencilView;
     
     ID3D11SamplerState *samplerStateLinearClamp;
     ID3D11SamplerState *samplerStateLinearWrap;
     ID3D11SamplerState* samplerStateNearestWrap;

     ID3D11RasterizerState *wireFrameRasterizer;
     ID3D11RasterizerState *fillRasterizerCullBack;
     ID3D11RasterizerState *fillRasterizerCullFront;
     ID3D11RasterizerState *fillRasterizerCullNone;

     ID3D11DepthStencilState *depthStencilOn;
     ID3D11DepthStencilState *depthStencilOff;
     ID3D11DepthStencilState *depthStencilOnWriteMaskZero;

     ID3D11BlendState *alphaBlendOn;
     ID3D11BlendState *alphaBlendOff;
     ID3D11BlendState *additiveBlending;
     ID3D11BlendState* subtractiveBlending;
     
     ObjectAllocator<VertexBufferD3D11> vertexBufferAllocator;
     ObjectAllocator<IndexBufferD3D11> indexBufferAllocator;
     ObjectAllocator<UniformBufferD3D11> uniformBufferAllocator;
     ObjectAllocator<FrameBufferD3D11> frameBufferAllocator;
     ObjectAllocator<VertexShaderD3D11> vertexShaderAllocator;
     ObjectAllocator<FragmentShaderD3D11> fragmentShaderAllocator;
     ObjectAllocator<TextureD3D11> textureAllocator;
     ObjectAllocator<BatchRendererD3D11> batchRendererAllocator;

     UniformBuffer* lightsUniformBuffer = nullptr;
     LightsUbo lighsUbo;

#if ANT_DEBUG
     DebugRendererD3D11 debugRenderer;
#endif
   
public:
     void Initialize(void *osWindow, i32 width, i32 height, i32 stackNum) override;
     void Shutdown() override;

     void OnResize(i32 width, i32 height) override;
     
     void BeginFrame(f32 r, f32 g, f32 b) override;
     void EndFrame(i32 vsync) override;

     void Draw(u32 verticesCount) override;
     void DrawIndexed(u32 indexCount) override;

     void SetRasterizerStateCullBack() override;
     void SetRasterizerStateCullFront() override;
     void SetRasterizerStateCullNone() override;
     void SetRasterizerStateWireframe() override;

     void SetDepthStencilOn() override;
     void SetDepthStencilOff() override;
     void SetDepthStencilOnWriteMaskZero() override;

     void SetAlphaBlending() override;
     void SetAdditiveBlending() override;
     void SetSubstractingBlending() override;
     void SetBlendingOff() override;
     
     VertexBuffer *VertexBufferAlloc(void *vertices, u32 count, u32 stride) override;
     void VertexBufferFree(VertexBuffer *vertexBuffer) override;
     void VertexBufferBind(VertexBuffer *vertexBuffer) override;

     IndexBuffer *IndexBufferAlloc(u32 *indices, u32 count) override;
     void IndexBufferFree(IndexBuffer *indexBuffer) override;
     void IndexBufferBind(IndexBuffer *indexBuffer) override;

     UniformBuffer *UniformBufferAlloc(u32 bindTo, void *data, u32 dataSize, u32 slot) override;
     void UniformBufferFree(UniformBuffer *uniformBuffer) override;
     void UniformBufferBind(UniformBuffer *uniformBuffer) override;
     void UniformBufferUpdate(UniformBuffer *uniformBuffer, void *data) override;

     VertexShader* VertexShaderAlloc(File file) override;
     void VertexShaderFree(VertexShader* shader) override;
     void VertexShaderBind(VertexShader* shader) override;

     FragmentShader* FragmentShaderAlloc(File file) override;
     void FragmentShaderFree(FragmentShader* shader) override;
     void FragmentShaderBind(FragmentShader* shader) override;


     Texture *TextureAlloc(const char *filepath) override;
     void TextureFree(Texture *texture) override;
     void TextureBind(Texture *texture, i32 slot) override;
     i32 TextureWidth(Texture *texture) override;
     i32 TextureHeight(Texture *texture) override;

     BatchRenderer* BatchRendererAlloc(VertexShader* vertShader, FragmentShader* fragShader, Texture* texture) override;
     void BatchRendererFree(BatchRenderer* batchRenderer) override;

     void SetDirectionalLight(const DirectionalLight& light) override;
     void AddPointLights(PointLight* lights, int count) override;
     int GetMaxPointLightCount() override;

     void UpdateViewPosition(const Vector3& viewPos) override;

     void DebugInit() override;
     void DebugTerminate() override;
     void DebugPresent() override;
     void DebugDrawLine(const Vector3& a, const Vector3& b, const Vector3& color) override;
     void DebugDrawSphere(const Vector3& c, f32 r, i32 hSlice, i32 vSlice, const Vector3& color) override;
     void DebugDrawCube(const Vector3& c, const Vector3& hExtend, const Vector3& color) override;

private:
     void CreateDeviceAndSwapChain();
     void CreateRenderTargetView();
     void CreateDepthStencilView(i32 width, i32 height);
     HRESULT CreateInputLayoutDescFromVertexShaderSignature(ID3DBlob* pShaderBlob, ID3D11Device* pD3DDevice,
                                                            ID3D11InputLayout** pInputLayout);
     void CreateSamplerStates();
     void CreateRasterizerStates();
     void CreateDepthStencilStates();
     void CreateBendingStates();

     void CreateLightsUniformBuffer();
     void DestroyLightsUniformBuffer();
};
