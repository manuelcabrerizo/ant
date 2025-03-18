#include <d3d11.h>
#include <d3dcompiler.h>

struct DirectXVertexBuffer : public VertexBuffer
{
     ID3D11Buffer *buffer;
     u32 verticesCount;
     u32 stride;
     u32 offset;
};

struct DirectXIndexBuffer : public IndexBuffer
{
     ID3D11Buffer *buffer;
     u32 indicesCount;
     DXGI_FORMAT format;
};

struct DirectXUniformBuffer : UniformBuffer
{
     ID3D11Buffer *buffer;
     u32 slot;
     u32 bindTo;
     u32 dataSize;
};

struct DirectXFrameBuffer : FrameBuffer
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

struct DirectXShader : Shader
{
     ID3D11InputLayout *layout;
     ID3D11VertexShader *vertexShader;
     ID3D11PixelShader *pixelShader;
};

struct DirectXTexture : Texture
{
     i32 w, h;
     ID3D11Texture2D *texture;
     ID3D11ShaderResourceView *shaderResourceView;
};

struct GraphicsManagerState
{
     HWND *window;
     i32 windowWidth;
     i32 windowHeight;
     
     ID3D11Device *device;
     ID3D11DeviceContext *deviceContext;
     IDXGISwapChain *swapChain;

     ID3D11RenderTargetView *renderTargetView;
     ID3D11DepthStencilView *depthStencilView;
     
     // TODO: ...
     ID3D11SamplerState *samplerStateLinearClamp;
     ID3D11SamplerState *samplerStateLinearWrap;

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
     
     ObjectAllocator<DirectXVertexBuffer> vertexBufferAllocator;
     ObjectAllocator<DirectXIndexBuffer> indexBufferAllocator;
     ObjectAllocator<DirectXUniformBuffer> uniformBufferAllocator;
     ObjectAllocator<DirectXFrameBuffer> frameBufferAllocator;
     ObjectAllocator<DirectXShader> shaderAllocator;
     ObjectAllocator<DirectXTexture> textureAllocator;
     
};

// State of the graphics manager
static GraphicsManagerState gGraphicsManagerState;
// Behavior of the graphics manager
GraphicsManager GraphicsManager::instance;
bool GraphicsManager::initialize = false;

static void DirectXCreateDeviceAndSwapChain(GraphicsManagerState *state)
{
     i32 deviceFlags = 0;
#ifdef ANT_DEBUG
     printf("Directx11 debug mode set\n");
     deviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

     D3D_DRIVER_TYPE driverTypes[] = {
          D3D_DRIVER_TYPE_HARDWARE,
          D3D_DRIVER_TYPE_WARP,
          D3D_DRIVER_TYPE_SOFTWARE
     };
     D3D_FEATURE_LEVEL featureLevels[] = {
          D3D_FEATURE_LEVEL_11_0,
          D3D_FEATURE_LEVEL_10_1,
          D3D_FEATURE_LEVEL_10_0
     };
     
     i32 driverTypesCount = ARRAY_LENGTH(driverTypes);
     i32 featureLevelsCount = ARRAY_LENGTH(featureLevels);

     DXGI_SWAP_CHAIN_DESC swapChainDesc;
     ZeroMemory(&swapChainDesc, sizeof(swapChainDesc));
     swapChainDesc.BufferCount = 2;
     swapChainDesc.BufferDesc.Width = state->windowWidth;
     swapChainDesc.BufferDesc.Height = state->windowHeight;
     swapChainDesc.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
     swapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
     swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
     swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
     swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
     swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
     swapChainDesc.OutputWindow = *state->window;
     swapChainDesc.Windowed = true;
     swapChainDesc.SampleDesc.Count = 1;
     swapChainDesc.SampleDesc.Quality = 0;
     swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

     D3D_FEATURE_LEVEL featureLevel;
     D3D_DRIVER_TYPE driverType;
     for(u32 driver = 0; driver < driverTypesCount; ++driver)
     {
          
          HRESULT result = D3D11CreateDeviceAndSwapChain(0, driverTypes[driver], 0, deviceFlags,
                                                           featureLevels, featureLevelsCount,
                                                           D3D11_SDK_VERSION, &swapChainDesc,
                                                           &state->swapChain,
                                                           &state->device,
                                                           &featureLevel,
                                                           &state->deviceContext);
          if(SUCCEEDED(result))
          {
               driverType = driverTypes[driver];
               printf("Driver selected %d\n", driver);
               break;
          }
     }
}


static void DirectXCreateRenderTargetView(GraphicsManagerState *state)
{
     // create render target view
     ID3D11Texture2D *backBufferTexture;
     state->swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void **)&backBufferTexture);
     if (FAILED(state->device->CreateRenderTargetView(backBufferTexture, 0, &state->renderTargetView)))
     {
          ASSERT(!"Error creating render target.");
     }
     backBufferTexture->Release();
}

static void DirectXCreateDepthStencilView(GraphicsManagerState *state)
{
     // TODO: remove this check, it does not belong to this function
     
     // check for msaa
     u32 msaaQuality4x;
     state->device->CheckMultisampleQualityLevels(DXGI_FORMAT_R8G8B8A8_UNORM, 4, &msaaQuality4x);
     if (msaaQuality4x <= 0)
     {
          ASSERT(!"Error msaa 4x not supported.");
     }
     
     // create the depth stencil texture
     ID3D11Texture2D *depthStencilTexture;
     D3D11_TEXTURE2D_DESC depthStencilTextureDesc;
     depthStencilTextureDesc.Width = state->windowWidth;
     depthStencilTextureDesc.Height = state->windowHeight;
     depthStencilTextureDesc.MipLevels = 1;
     depthStencilTextureDesc.ArraySize = 1;
     depthStencilTextureDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
     depthStencilTextureDesc.SampleDesc.Count = 1;
     depthStencilTextureDesc.SampleDesc.Quality = 0;
     depthStencilTextureDesc.Usage = D3D11_USAGE_DEFAULT;
     depthStencilTextureDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
     depthStencilTextureDesc.CPUAccessFlags = 0;
     depthStencilTextureDesc.MiscFlags = 0;
     if (FAILED(state->device->CreateTexture2D(&depthStencilTextureDesc, 0, &depthStencilTexture)))
     {
          ASSERT(!"Error creating depth stencil texture.");
     }
     if (FAILED(state->device->CreateDepthStencilView(depthStencilTexture, 0, &state->depthStencilView)))
     {
          ASSERT(!"Error creating depth stencil view.");
     }
     depthStencilTexture->Release();
}
// TODO: refactor this code
static HRESULT DirectXCreateInputLayoutDescFromVertexShaderSignature(ID3DBlob* pShaderBlob, ID3D11Device* pD3DDevice, ID3D11InputLayout** pInputLayout )
{
     GraphicsManagerState *state = &gGraphicsManagerState;
     // Reflect shader info
     ID3D11ShaderReflection* pVertexShaderReflection = NULL;
     
     if(FAILED(D3DReflect(pShaderBlob->GetBufferPointer(), pShaderBlob->GetBufferSize(), IID_ID3D11ShaderReflection, (void**)&pVertexShaderReflection)))
     {
          return S_FALSE;
     }
 
     // Get shader info
     D3D11_SHADER_DESC shaderDesc;
     pVertexShaderReflection->GetDesc( &shaderDesc );
 
     // Read input layout description from shader info
     Frame frame = MemoryManager::Get()->GetFrame(FRAME_MEMORY);

     Array<D3D11_INPUT_ELEMENT_DESC> inputLayoutDesc;
     inputLayoutDesc.Init(shaderDesc.InputParameters, FRAME_MEMORY);
     
     for ( uint32 i=0; i< shaderDesc.InputParameters; i++ )
     {
          D3D11_SIGNATURE_PARAMETER_DESC paramDesc;
          pVertexShaderReflection->GetInputParameterDesc(i, &paramDesc );
 
          // fill out input element desc
          D3D11_INPUT_ELEMENT_DESC elementDesc;
          elementDesc.SemanticName = paramDesc.SemanticName;
          elementDesc.SemanticIndex = paramDesc.SemanticIndex;
          elementDesc.InputSlot = 0;
          elementDesc.AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
          elementDesc.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
          elementDesc.InstanceDataStepRate = 0;   
 
          // determine DXGI format
          if ( paramDesc.Mask == 1 )
          {
               if ( paramDesc.ComponentType == D3D_REGISTER_COMPONENT_UINT32 ) elementDesc.Format = DXGI_FORMAT_R32_UINT;
               else if ( paramDesc.ComponentType == D3D_REGISTER_COMPONENT_SINT32 ) elementDesc.Format = DXGI_FORMAT_R32_SINT;
               else if ( paramDesc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32 ) elementDesc.Format = DXGI_FORMAT_R32_FLOAT;
          }
          else if ( paramDesc.Mask <= 3 )
          {
               if ( paramDesc.ComponentType == D3D_REGISTER_COMPONENT_UINT32 ) elementDesc.Format = DXGI_FORMAT_R32G32_UINT;
               else if ( paramDesc.ComponentType == D3D_REGISTER_COMPONENT_SINT32 ) elementDesc.Format = DXGI_FORMAT_R32G32_SINT;
               else if ( paramDesc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32 ) elementDesc.Format = DXGI_FORMAT_R32G32_FLOAT;
          }
          else if ( paramDesc.Mask <= 7 )
          {
               if ( paramDesc.ComponentType == D3D_REGISTER_COMPONENT_UINT32 ) elementDesc.Format = DXGI_FORMAT_R32G32B32_UINT;
               else if ( paramDesc.ComponentType == D3D_REGISTER_COMPONENT_SINT32 ) elementDesc.Format = DXGI_FORMAT_R32G32B32_SINT;
               else if ( paramDesc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32 ) elementDesc.Format = DXGI_FORMAT_R32G32B32_FLOAT;
          }
          else if ( paramDesc.Mask <= 15 )
          {
               if ( paramDesc.ComponentType == D3D_REGISTER_COMPONENT_UINT32 ) elementDesc.Format = DXGI_FORMAT_R32G32B32A32_UINT;
               else if ( paramDesc.ComponentType == D3D_REGISTER_COMPONENT_SINT32 ) elementDesc.Format = DXGI_FORMAT_R32G32B32A32_SINT;
               else if ( paramDesc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32 ) elementDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
          }
 
          //save element desc
          inputLayoutDesc.Push(elementDesc);
     }       
 
     // Try to create Input Layout
     HRESULT hr = state->device->CreateInputLayout(inputLayoutDesc.data, inputLayoutDesc.size, pShaderBlob->GetBufferPointer(), pShaderBlob->GetBufferSize(), pInputLayout);

     MemoryManager::Get()->ReleaseFrame(frame);
     
     //Free allocation shader reflection memory
     pVertexShaderReflection->Release();
     return hr;
}

static void DirectXCreateSamplerStates(GraphicsManagerState *state)
{
     D3D11_SAMPLER_DESC colorMapDesc = {};
     colorMapDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
     colorMapDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
     colorMapDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
     colorMapDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
     colorMapDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
     colorMapDesc.MaxLOD = D3D11_FLOAT32_MAX;
     if (FAILED(state->device->CreateSamplerState(&colorMapDesc, &state->samplerStateLinearClamp)))
     {
          ASSERT(!"Error: Failed Creating sampler state Point\n");
     }
     colorMapDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
     colorMapDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
     colorMapDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
     if (FAILED(state->device->CreateSamplerState(&colorMapDesc, &state->samplerStateLinearWrap)))
     {
          ASSERT(!"Error: Failed Creating sampler state Linear\n");
     }
}

static void DirectX11CreateRasterizerStates(GraphicsManagerState *state)
{
     D3D11_RASTERIZER_DESC fillRasterizerFrontDesc;
     ZeroMemory(&fillRasterizerFrontDesc, sizeof(fillRasterizerFrontDesc));
     fillRasterizerFrontDesc.FillMode = D3D11_FILL_SOLID;
     fillRasterizerFrontDesc.CullMode = D3D11_CULL_FRONT;
     fillRasterizerFrontDesc.FrontCounterClockwise = true;
     fillRasterizerFrontDesc.DepthClipEnable = true;
     fillRasterizerFrontDesc.AntialiasedLineEnable = true;
     fillRasterizerFrontDesc.MultisampleEnable = true;
     state->device->CreateRasterizerState(&fillRasterizerFrontDesc, &state->fillRasterizerCullFront);

     D3D11_RASTERIZER_DESC fillRasterizerBackDesc;
     ZeroMemory(&fillRasterizerBackDesc, sizeof(fillRasterizerBackDesc));
     fillRasterizerBackDesc.FillMode = D3D11_FILL_SOLID;
     fillRasterizerBackDesc.CullMode = D3D11_CULL_BACK;
     fillRasterizerBackDesc.FrontCounterClockwise = true;
     fillRasterizerBackDesc.DepthClipEnable = true;
     fillRasterizerBackDesc.AntialiasedLineEnable = true;
     fillRasterizerBackDesc.MultisampleEnable = true;
     state->device->CreateRasterizerState(&fillRasterizerBackDesc, &state->fillRasterizerCullBack);

     D3D11_RASTERIZER_DESC fillRasterizerNoneDesc;
     ZeroMemory(&fillRasterizerNoneDesc, sizeof(fillRasterizerNoneDesc));
     fillRasterizerNoneDesc.FillMode = D3D11_FILL_SOLID;
     fillRasterizerNoneDesc.CullMode = D3D11_CULL_NONE;
     fillRasterizerNoneDesc.FrontCounterClockwise = true;
     fillRasterizerNoneDesc.DepthClipEnable = true;
     fillRasterizerNoneDesc.AntialiasedLineEnable = true;
     fillRasterizerNoneDesc.MultisampleEnable = true;
     state->device->CreateRasterizerState(&fillRasterizerNoneDesc, &state->fillRasterizerCullNone);

     D3D11_RASTERIZER_DESC wireFrameRasterizerDesc;
     ZeroMemory(&wireFrameRasterizerDesc, sizeof(wireFrameRasterizerDesc));
     wireFrameRasterizerDesc.FillMode = D3D11_FILL_WIREFRAME;
     wireFrameRasterizerDesc.CullMode = D3D11_CULL_NONE;
     wireFrameRasterizerDesc.FrontCounterClockwise = true;
     wireFrameRasterizerDesc.DepthClipEnable = true;
     wireFrameRasterizerDesc.AntialiasedLineEnable = true;
     wireFrameRasterizerDesc.MultisampleEnable = true;
     state->device->CreateRasterizerState(&wireFrameRasterizerDesc, &state->wireFrameRasterizer);
}

void GraphicsManager::Init(void *osWindow, i32 width, i32 height, i32 stackNum)
{
     if(initialize)
     {
          ASSERT(!"Error: graphics manager already initialize");
     }
     
     GraphicsManagerState *state = &gGraphicsManagerState;
     state->window = (HWND *)osWindow;
     state->windowWidth = width;
     state->windowHeight = height;
     
     DirectXCreateDeviceAndSwapChain(state);
     
     DirectXCreateRenderTargetView(state);
     DirectXCreateDepthStencilView(state);
     DirectXCreateSamplerStates(state);
     DirectX11CreateRasterizerStates(state);

     //  Default renderer settings
     state->deviceContext->OMSetRenderTargets(1, &state->renderTargetView, state->depthStencilView);
     state->deviceContext->PSSetSamplers(0, 1, &state->samplerStateLinearWrap);

     D3D11_VIEWPORT viewport;
     viewport.TopLeftX = 0;
     viewport.TopLeftY = 0;
     viewport.Width = (f32)width;
     viewport.Height = (f32)height;
     viewport.MinDepth = 0.0f;
     viewport.MaxDepth = 1.0f;
     state->deviceContext->RSSetViewports(1, &viewport);
        
     state->vertexBufferAllocator.Init(stackNum);
     state->indexBufferAllocator.Init(stackNum);
     state->uniformBufferAllocator.Init(stackNum);
     state->frameBufferAllocator.Init(stackNum);
     state->shaderAllocator.Init(stackNum);
     state->textureAllocator.Init(stackNum);

     initialize = true;
     printf("DirectX11 Initialized!\n");

     GraphicsManager::Get()->SetRasterizerStateCullNone();
}

void GraphicsManager::Terminate()
{
     if(!initialize)
     {
          ASSERT(!"Error: graphics manager has not been initialize");
     }
     
     GraphicsManagerState *state = &gGraphicsManagerState;
     
     //Sleep(1000);
     state->deviceContext->Flush();
          
     state->wireFrameRasterizer->Release();
     state->fillRasterizerCullBack->Release();
     state->fillRasterizerCullFront->Release();
     state->fillRasterizerCullNone->Release();
     
     state->samplerStateLinearClamp->Release();
     state->samplerStateLinearWrap->Release();
     
     state->depthStencilView->Release();
     state->renderTargetView->Release();
     state->swapChain->Release();
     state->deviceContext->Release();
     state->device->Release();

     initialize = false;
     printf("DirectX11 Terminated!\n");
}

GraphicsManager *GraphicsManager::Get()
{
     if(!initialize)
     {
          ASSERT(!"Error: graphics manager has not been initialize");
     }
     return &instance;
}


void GraphicsManager::BeginFrame(f32 r, f32 g, f32 b)
{
     GraphicsManagerState *state = &gGraphicsManagerState;
     f32 clearColor[] = { r, g, b, 1.0f };
     state->deviceContext->ClearRenderTargetView(state->renderTargetView, clearColor);
     state->deviceContext->ClearDepthStencilView(state->depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
}

void GraphicsManager::EndFrame(i32 vsync)
{
     GraphicsManagerState *state = &gGraphicsManagerState;
     state->swapChain->Present(vsync, 0);
}


void GraphicsManager::Draw(u32 verticesCount)
{
     GraphicsManagerState *state = &gGraphicsManagerState;
     state->deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
     state->deviceContext->Draw(verticesCount, 0);
}

void GraphicsManager::DrawIndexed(u32 indexCount)
{
     GraphicsManagerState *state = &gGraphicsManagerState;
     state->deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
     state->deviceContext->DrawIndexed(indexCount, 0, 0);
}

void GraphicsManager::SetRasterizerStateCullBack()
{
     GraphicsManagerState *state = &gGraphicsManagerState;
     state->deviceContext->RSSetState(state->fillRasterizerCullBack);
}

void GraphicsManager::SetRasterizerStateCullFront()
{
     GraphicsManagerState *state = &gGraphicsManagerState;
     state->deviceContext->RSSetState(state->fillRasterizerCullFront);
}

void GraphicsManager::SetRasterizerStateCullNone()
{
     GraphicsManagerState *state = &gGraphicsManagerState;
     state->deviceContext->RSSetState(state->fillRasterizerCullNone);
}

void GraphicsManager::SetRasterizerStateWireframe()
{
     GraphicsManagerState *state = &gGraphicsManagerState;
     state->deviceContext->RSSetState(state->wireFrameRasterizer);
}

VertexBuffer *GraphicsManager::VertexBufferAlloc(void *vertices, u32 count, u32 stride)
{
     GraphicsManagerState *state = &gGraphicsManagerState;
     DirectXVertexBuffer *vb = state->vertexBufferAllocator.Alloc();
     vb->verticesCount = count;
     vb->stride = stride;
     vb->offset = 0;
 
     D3D11_BUFFER_DESC vertexDesc;
     ZeroMemory(&vertexDesc, sizeof(vertexDesc));
     vertexDesc.Usage = D3D11_USAGE_DEFAULT;
     vertexDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
     vertexDesc.ByteWidth = stride * count;

     D3D11_SUBRESOURCE_DATA subresourceData;
     ZeroMemory(&subresourceData, sizeof(subresourceData));
     subresourceData.pSysMem = vertices;
     if (FAILED(state->device->CreateBuffer(&vertexDesc, &subresourceData, &vb->buffer)))
     {
          ASSERT(!"Error creating vertex buffer");
     }
     return (VertexBuffer *)vb;
}

void GraphicsManager::VertexBufferFree(VertexBuffer *vertexBuffer)
{
     GraphicsManagerState *state = &gGraphicsManagerState;
     DirectXVertexBuffer *vb = (DirectXVertexBuffer *)vertexBuffer;
     vb->buffer->Release();
     state->vertexBufferAllocator.Free(vb);
}

void GraphicsManager::VertexBufferBind(VertexBuffer *vertexBuffer)
{
     GraphicsManagerState *state = &gGraphicsManagerState;
     DirectXVertexBuffer *vb = (DirectXVertexBuffer *)vertexBuffer;
     state->deviceContext->IASetVertexBuffers(0, 1, &vb->buffer, &vb->stride, &vb->offset);
}

IndexBuffer *GraphicsManager::IndexBufferAlloc(u32 *indices, u32 count)
{
     GraphicsManagerState *state = &gGraphicsManagerState;
     DirectXIndexBuffer *ib = state->indexBufferAllocator.Alloc();
     ib->indicesCount = count;
     ib->format = DXGI_FORMAT_R32_UINT;

     D3D11_BUFFER_DESC indexDesc;
     ZeroMemory(&indexDesc, sizeof(indexDesc));
     indexDesc.Usage = D3D11_USAGE_IMMUTABLE;
     indexDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
     indexDesc.ByteWidth = sizeof(u32) * count;

     D3D11_SUBRESOURCE_DATA subresourceData;
     ZeroMemory(&subresourceData, sizeof(subresourceData));
     subresourceData.pSysMem = indices;
     if (FAILED(state->device->CreateBuffer(&indexDesc, &subresourceData, &ib->buffer)))
     {
          ASSERT(!"Error creating vertex buffer");
     }
     
     return (IndexBuffer *)ib;
}

void GraphicsManager::IndexBufferFree(IndexBuffer *indexBuffer)
{
     GraphicsManagerState *state = &gGraphicsManagerState;
     DirectXIndexBuffer *ib = (DirectXIndexBuffer *)indexBuffer;
     ib->buffer->Release();
     state->indexBufferAllocator.Free(ib);
}

void GraphicsManager::IndexBufferBind(IndexBuffer *indexBuffer)
{
     GraphicsManagerState *state = &gGraphicsManagerState;
     DirectXIndexBuffer *ib = (DirectXIndexBuffer *)indexBuffer;
     state->deviceContext->IASetIndexBuffer(ib->buffer, ib->format, 0);
}

UniformBuffer *GraphicsManager::UniformBufferAlloc(u32 bindTo, void *data, u32 dataSize, u32 slot)
{
     GraphicsManagerState *state = &gGraphicsManagerState;
     DirectXUniformBuffer *ub = state->uniformBufferAllocator.Alloc();
     ub->slot = slot;
     ub->bindTo = bindTo;
     ub->dataSize = dataSize;

     D3D11_BUFFER_DESC bufferDesc;
     ZeroMemory(&bufferDesc, sizeof(bufferDesc));
     bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
     bufferDesc.ByteWidth = dataSize;
     bufferDesc.Usage = D3D11_USAGE_DEFAULT;
     if (FAILED(state->device->CreateBuffer(&bufferDesc, 0, &ub->buffer)))
     {
          ASSERT(!"Error creating const buffer");
     }

     state->deviceContext->UpdateSubresource(ub->buffer, 0, 0, data, 0, 0);

     return (UniformBuffer *)ub;
}

void GraphicsManager::UniformBufferFree(UniformBuffer *uniformBuffer)
{
     GraphicsManagerState *state = &gGraphicsManagerState;
     DirectXUniformBuffer *ub = (DirectXUniformBuffer *)uniformBuffer;
     ub->buffer->Release();
     state->uniformBufferAllocator.Free(ub);
}

void GraphicsManager::UniformBufferBind(UniformBuffer *uniformBuffer)
{
     GraphicsManagerState *state = &gGraphicsManagerState;
     DirectXUniformBuffer *ub = (DirectXUniformBuffer *)uniformBuffer;
     
     if (ub->bindTo & BIND_TO_VS)
     {
          state->deviceContext->VSSetConstantBuffers(ub->slot, 1, &ub->buffer);
     }
     if (ub->bindTo & BIND_TO_PS)
     {
          state->deviceContext->PSSetConstantBuffers(ub->slot, 1, &ub->buffer);
     }
     if (ub->bindTo & BIND_TO_GS)
     {
          state->deviceContext->GSSetConstantBuffers(ub->slot, 1, &ub->buffer);
     }
}

void GraphicsManager::UniformBufferUpdate(UniformBuffer *uniformBuffer, void *data)
{
     // TODO: mabye use Map and Unmap instead of UpdateSubresource
     GraphicsManagerState *state = &gGraphicsManagerState;
     DirectXUniformBuffer *ub = (DirectXUniformBuffer *)uniformBuffer;
     state->deviceContext->UpdateSubresource(ub->buffer, 0, 0, data, 0, 0);
}


Shader *GraphicsManager::ShaderAlloc(File vertFile, File fragFile)
{
     GraphicsManagerState *state = &gGraphicsManagerState;
     DirectXShader *sh = state->shaderAllocator.Alloc();

     ID3DBlob *errorShader;
     ID3DBlob *vertShaderCompiled;
     ID3DBlob *fragShaderCompiled;
     // Compile vertex Shader
     D3DCompile(vertFile.data, vertFile.size,
                0, 0, 0, "vs_main", "vs_5_0",
                D3DCOMPILE_ENABLE_STRICTNESS, 0,
                &vertShaderCompiled, &errorShader);
     if (errorShader != 0)
     {
          char* errorString = (char*)errorShader->GetBufferPointer();
          printf("Error compiling VERTEX SHADER\n");
          printf("%s\n", errorString);
          errorShader->Release();
     }
     else
     {
          state->device->CreateVertexShader(
               vertShaderCompiled->GetBufferPointer(),
               vertShaderCompiled->GetBufferSize(), 0,
               &sh->vertexShader);
     }
     // Create the input layout for this shader
     if(FAILED(DirectXCreateInputLayoutDescFromVertexShaderSignature(vertShaderCompiled, state->device, &sh->layout)))
     {
          ASSERT(!"Error creating input layout for this shader");
     }
     // Compile fragment Shader
     D3DCompile(fragFile.data, fragFile.size,
                0, 0, 0, "fs_main", "ps_5_0",
                D3DCOMPILE_ENABLE_STRICTNESS, 0,
                &fragShaderCompiled, &errorShader);
     if (errorShader != 0)
     {
          char* errorString = (char*)errorShader->GetBufferPointer();
          printf("Error compiling FRAGMENT SHADER\n");
          printf("%s\n", errorString);
          errorShader->Release();
     }
     else
     {
          state->device->CreatePixelShader(
               fragShaderCompiled->GetBufferPointer(),
               fragShaderCompiled->GetBufferSize(), 0,
               &sh->pixelShader);
     }

     vertShaderCompiled->Release();
     fragShaderCompiled->Release();

     return (Shader *)sh;
}

void GraphicsManager::ShaderFree(Shader *shader)
{
     GraphicsManagerState *state = &gGraphicsManagerState;
     DirectXShader *sh = (DirectXShader *)shader;
     sh->layout->Release();
     sh->vertexShader->Release();
     sh->pixelShader->Release();
     state->shaderAllocator.Free(sh);

}

void GraphicsManager::ShaderBind(Shader *shader)
{
     GraphicsManagerState *state = &gGraphicsManagerState;
     DirectXShader *sh = (DirectXShader *)shader;
     state->deviceContext->IASetInputLayout(sh->layout);
     state->deviceContext->VSSetShader(sh->vertexShader, 0, 0);
     state->deviceContext->PSSetShader(sh->pixelShader, 0, 0);
}

Texture *GraphicsManager::TextureAlloc(const char *filepath)
{
     GraphicsManagerState *state = &gGraphicsManagerState;
     DirectXTexture *tx = state->textureAllocator.Alloc();

     stbi_set_flip_vertically_on_load(true);

     int channels;
     void* data = (void*)stbi_load(filepath, &tx->w, &tx->h, &channels, 0);
     if (!data)
     {
          ASSERT(!"Error reading texture file");
     }

     D3D11_SUBRESOURCE_DATA subresourceData = {};
     subresourceData.pSysMem = data;
     subresourceData.SysMemPitch = tx->w * channels;

     D3D11_TEXTURE2D_DESC texDesc = {};
     texDesc.Width = tx->w;
     texDesc.Height = tx->h;
     texDesc.MipLevels = 0;
     texDesc.ArraySize = 1;
     texDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
     texDesc.SampleDesc.Count = 1;
     texDesc.SampleDesc.Quality = 0;
     texDesc.Usage = D3D11_USAGE_DEFAULT;
     texDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
     texDesc.CPUAccessFlags = 0;
     texDesc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;

     if (FAILED(state->device->CreateTexture2D(&texDesc, 0, &tx->texture)))
     {
          ASSERT(!"Error creating texture");
     }

     state->deviceContext->UpdateSubresource(tx->texture, 0, 0, subresourceData.pSysMem, subresourceData.SysMemPitch, 0);

     D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
     srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
     srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
     srvDesc.Texture2D.MipLevels = -1;
     srvDesc.Texture2D.MostDetailedMip = 0;
     if (FAILED(state->device->CreateShaderResourceView(tx->texture, &srvDesc, &tx->shaderResourceView)))
     {
          ASSERT(!"Error creating texture shader resource view");
     }

     state->deviceContext->GenerateMips(tx->shaderResourceView);

     stbi_image_free(data);

     return (Texture *)tx;
}

void GraphicsManager::TextureFree(Texture *texture)
{
     GraphicsManagerState *state = &gGraphicsManagerState;
     DirectXTexture *tx = (DirectXTexture *)texture;
     tx->texture->Release();
     tx->shaderResourceView->Release();
     state->textureAllocator.Free(tx);
}

void GraphicsManager::TextureBind(Texture *texture, i32 slot)
{
     GraphicsManagerState *state = &gGraphicsManagerState;
     DirectXTexture *tx = (DirectXTexture *)texture;
     state->deviceContext->PSSetShaderResources(slot, 1, &tx->shaderResourceView);
}

i32 GraphicsManager::TextureWidth(Texture *texture)
{
     GraphicsManagerState *state = &gGraphicsManagerState;
     DirectXTexture *tx = (DirectXTexture *)texture;
     return tx->w;
}

i32 GraphicsManager::TextureHeight(Texture *texture)
{
     GraphicsManagerState *state = &gGraphicsManagerState;
     DirectXTexture *tx = (DirectXTexture *)texture;
     return tx->h;
}

