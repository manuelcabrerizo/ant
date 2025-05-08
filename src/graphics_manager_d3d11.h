#include <d3d11_1.h>
#include <d3dcompiler.h>

#include "debug_renderer_d3d11.h"

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

struct ShaderD3D11 : Shader
{
     ID3D11InputLayout *layout;
     ID3D11VertexShader *vertexShader;
     ID3D11PixelShader *pixelShader;
};

struct TextureD3D11 : Texture
{
     i32 w, h;
     ID3D11Texture2D *texture;
     ID3D11ShaderResourceView *shaderResourceView;
};

class GraphicsManagerD3D11 : public GraphicsManager
{
private:     
     HWND *window;
     i32 windowWidth;
     i32 windowHeight;
     DXGI_SWAP_CHAIN_DESC sd;
     
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

     ID3D11RasterizerState *wireFrameRasterizer;
     ID3D11RasterizerState *fillRasterizerCullBack;
     ID3D11RasterizerState *fillRasterizerCullFront;
     ID3D11RasterizerState *fillRasterizerCullNone;

     // TODO: ...
     ID3D11DepthStencilState *depthStencilOn;
     ID3D11DepthStencilState *depthStencilOff;
     ID3D11DepthStencilState *depthStencilOnWriteMaskZero;

     ID3D11BlendState *alphaBlendOn;
     ID3D11BlendState *alphaBlendOff;
     ID3D11BlendState *additiveBlending;
     
     ObjectAllocator<VertexBufferD3D11> vertexBufferAllocator;
     ObjectAllocator<IndexBufferD3D11> indexBufferAllocator;
     ObjectAllocator<UniformBufferD3D11> uniformBufferAllocator;
     ObjectAllocator<FrameBufferD3D11> frameBufferAllocator;
     ObjectAllocator<ShaderD3D11> shaderAllocator;
     ObjectAllocator<TextureD3D11> textureAllocator;

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

     Shader *ShaderAlloc(File vertFilepath, File fragFilepath) override;
     void ShaderFree(Shader *shader) override;
     void ShaderBind(Shader *shader) override;

     Texture *TextureAlloc(const char *filepath) override;
     void TextureFree(Texture *texture) override;
     void TextureBind(Texture *texture, i32 slot) override;
     i32 TextureWidth(Texture *texture) override;
     i32 TextureHeight(Texture *texture) override;

     void DebugInit() override;
     void DebugTerminate() override;
     void DebugPresent() override;
     void DebugDrawLine(Vector3& a, Vector3& b) override;
     void DebugDrawSphere(Vector3& c, f32 r, i32 hSlice, i32 vSlice) override;
     void DebugDrawCube(Vector3& c, Vector3& hExtend) override;

private:
     void CreateDeviceAndSwapChain();
     void CreateRenderTargetView();
     void CreateDepthStencilView(i32 width, i32 height);
     HRESULT CreateInputLayoutDescFromVertexShaderSignature(ID3DBlob* pShaderBlob, ID3D11Device* pD3DDevice,
                                                            ID3D11InputLayout** pInputLayout);
     void CreateSamplerStates();
     void CreateRasterizerStates();
};
