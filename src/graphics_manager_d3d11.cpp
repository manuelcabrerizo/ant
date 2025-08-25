#include <graphics_manager_d3d11.h>
#include <graphics_manager.h>
#include <memory_manager.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <math/algebra.h>
#include <math/quaternion.h>

#include <dxgidebug.h>

void GraphicsManagerD3D11::Initialize(void *osWindow, i32 width, i32 height, i32 stackNum)
{    
     window = (HWND *)osWindow;
     windowWidth = width;
     windowHeight = height;
     
     CreateDeviceAndSwapChain();
     CreateRenderTargetView();
     CreateDepthStencilView(windowWidth, windowHeight);
     CreateSamplerStates();
     CreateRasterizerStates();

     //  Default renderer settings
     deviceContext->OMSetRenderTargets(1, &renderTargetView, depthStencilView);
     deviceContext->PSSetSamplers(0, 1, &samplerStateLinearWrap);

     D3D11_VIEWPORT viewport;
     viewport.TopLeftX = 0;
     viewport.TopLeftY = 0;
     viewport.Width = (f32)width;
     viewport.Height = (f32)height;
     viewport.MinDepth = 0.0f;
     viewport.MaxDepth = 1.0f;
     deviceContext->RSSetViewports(1, &viewport);
     
     vertexBufferAllocator.Init(stackNum);
     indexBufferAllocator.Init(stackNum);
     uniformBufferAllocator.Init(stackNum);
     frameBufferAllocator.Init(stackNum);
     vertexShaderAllocator.Init(stackNum);
     fragmentShaderAllocator.Init(stackNum);
     textureAllocator.Init(stackNum);

     printf("DirectX11 Initialized!\n");

     SetRasterizerStateCullBack();

     //swapChain->SetFullscreenState(TRUE, nullptr);
}

#define DXGI_GET_DEBUG_INTERFACE(name) HRESULT name(REFIID riid, void** ppDebug)
typedef DXGI_GET_DEBUG_INTERFACE(DXGIGetDebugInterfacePtr);

void GraphicsManagerD3D11::Shutdown()
{
     deviceContext->Flush();
     deviceContext->ClearState();
     swapChain->SetFullscreenState(FALSE, nullptr);

     wireFrameRasterizer->Release();
     fillRasterizerCullBack->Release();
     fillRasterizerCullFront->Release();
     fillRasterizerCullNone->Release();
     
     samplerStateLinearClamp->Release();
     samplerStateLinearWrap->Release();
     
     depthStencilView->Release();
     renderTargetView->Release();
  
     if(swapChain1) swapChain1->Release();
     swapChain->Release();
     if(deviceContext1) deviceContext1->Release();
     deviceContext->Release();
     if(device1) device1->Release();
     device->Release();

     HMODULE DxgiDebugLibrary = LoadLibraryA("DXGIDebug.dll");
     if (DxgiDebugLibrary)
     {
         DXGIGetDebugInterfacePtr* DXGIGetDebugInterface =
             (DXGIGetDebugInterfacePtr*)GetProcAddress(DxgiDebugLibrary, "DXGIGetDebugInterface");

         IDXGIDebug* debug = nullptr;
         DXGIGetDebugInterface(__uuidof(IDXGIDebug), (void**)&debug);

         debug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_ALL);

         debug->Release();
     }

     printf("DirectX11 Terminated!\n");
}

void GraphicsManagerD3D11::OnResize(i32 width, i32 height)
{
     windowWidth = width;
     windowHeight = height;

     // Release the render target view
     deviceContext->OMSetRenderTargets(0, 0, 0);
     renderTargetView->Release();
     depthStencilView->Release();

     swapChain->ResizeBuffers(0, 0, 0, DXGI_FORMAT_UNKNOWN, 0);

     // create render target view
     CreateRenderTargetView();
     CreateDepthStencilView(width, height);

     deviceContext->OMSetRenderTargets(1, &renderTargetView, depthStencilView);

     // Set up the viewport.
     D3D11_VIEWPORT vp;
     vp.Width = (f32)width;
     vp.Height = (f32)height;
     vp.MinDepth = 0.0f;
     vp.MaxDepth = 1.0f;
     vp.TopLeftX = 0;
     vp.TopLeftY = 0;
     deviceContext->RSSetViewports( 1, &vp );
}

void GraphicsManagerD3D11::BeginFrame(f32 r, f32 g, f32 b)
{
     f32 clearColor[] = { r, g, b, 1.0f };
     deviceContext->ClearRenderTargetView(renderTargetView, clearColor);
     deviceContext->ClearDepthStencilView(depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
}

void GraphicsManagerD3D11::EndFrame(i32 vsync)
{
     swapChain->Present(vsync, 0);
}


void GraphicsManagerD3D11::Draw(u32 verticesCount)
{
     deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
     deviceContext->Draw(verticesCount, 0);
}

void GraphicsManagerD3D11::DrawIndexed(u32 indexCount)
{
     deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
     deviceContext->DrawIndexed(indexCount, 0, 0);
}

void GraphicsManagerD3D11::SetRasterizerStateCullBack()
{
     deviceContext->RSSetState(fillRasterizerCullBack);
}

void GraphicsManagerD3D11::SetRasterizerStateCullFront()
{
     deviceContext->RSSetState(fillRasterizerCullFront);
}

void GraphicsManagerD3D11::SetRasterizerStateCullNone()
{
     deviceContext->RSSetState(fillRasterizerCullNone);
}

void GraphicsManagerD3D11::SetRasterizerStateWireframe()
{
     deviceContext->RSSetState(wireFrameRasterizer);
}

VertexBuffer *GraphicsManagerD3D11::VertexBufferAlloc(void *vertices, u32 count, u32 stride)
{
     VertexBufferD3D11 *vb = vertexBufferAllocator.Alloc();
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
     if (FAILED(device->CreateBuffer(&vertexDesc, &subresourceData, &vb->buffer)))
     {
          ASSERT(!"Error creating vertex buffer");
     }
     return (VertexBuffer *)vb;
}

void GraphicsManagerD3D11::VertexBufferFree(VertexBuffer *vertexBuffer)
{
     VertexBufferD3D11 *vb = (VertexBufferD3D11 *)vertexBuffer;
     vb->buffer->Release();
     vertexBufferAllocator.Free(vb);
}

void GraphicsManagerD3D11::VertexBufferBind(VertexBuffer *vertexBuffer)
{
     VertexBufferD3D11 *vb = (VertexBufferD3D11 *)vertexBuffer;
     deviceContext->IASetVertexBuffers(0, 1, &vb->buffer, &vb->stride, &vb->offset);
}

IndexBuffer *GraphicsManagerD3D11::IndexBufferAlloc(u32 *indices, u32 count)
{
     IndexBufferD3D11 *ib = indexBufferAllocator.Alloc();
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
     if (FAILED(device->CreateBuffer(&indexDesc, &subresourceData, &ib->buffer)))
     {
          ASSERT(!"Error creating vertex buffer");
     }
     
     return (IndexBuffer *)ib;
}

void GraphicsManagerD3D11::IndexBufferFree(IndexBuffer *indexBuffer)
{
     IndexBufferD3D11 *ib = (IndexBufferD3D11 *)indexBuffer;
     ib->buffer->Release();
     indexBufferAllocator.Free(ib);
}

void GraphicsManagerD3D11::IndexBufferBind(IndexBuffer *indexBuffer)
{
     IndexBufferD3D11 *ib = (IndexBufferD3D11 *)indexBuffer;
     deviceContext->IASetIndexBuffer(ib->buffer, ib->format, 0);
}

UniformBuffer *GraphicsManagerD3D11::UniformBufferAlloc(u32 bindTo, void *data, u32 dataSize, u32 slot)
{
     UniformBufferD3D11 *ub = uniformBufferAllocator.Alloc();
     ub->slot = slot;
     ub->bindTo = bindTo;
     ub->dataSize = dataSize;

     D3D11_BUFFER_DESC bufferDesc;
     ZeroMemory(&bufferDesc, sizeof(bufferDesc));
     bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
     bufferDesc.ByteWidth = dataSize;
     bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
     bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

     D3D11_SUBRESOURCE_DATA subresourceData;
     ZeroMemory(&subresourceData, sizeof(subresourceData));
     subresourceData.pSysMem = data;

     if (FAILED(device->CreateBuffer(&bufferDesc, &subresourceData, &ub->buffer)))
     {
          ASSERT(!"Error creating const buffer");
     }

     return (UniformBuffer *)ub;
}

void GraphicsManagerD3D11::UniformBufferFree(UniformBuffer *uniformBuffer)
{
     UniformBufferD3D11 *ub = (UniformBufferD3D11 *)uniformBuffer;
     ub->buffer->Release();
     uniformBufferAllocator.Free(ub);
}

void GraphicsManagerD3D11::UniformBufferBind(UniformBuffer *uniformBuffer)
{
     UniformBufferD3D11 *ub = (UniformBufferD3D11 *)uniformBuffer;
     
     if (ub->bindTo & BIND_TO_VS)
     {
          deviceContext->VSSetConstantBuffers(ub->slot, 1, &ub->buffer);
     }
     if (ub->bindTo & BIND_TO_PS)
     {
          deviceContext->PSSetConstantBuffers(ub->slot, 1, &ub->buffer);
     }
     if (ub->bindTo & BIND_TO_GS)
     {
          deviceContext->GSSetConstantBuffers(ub->slot, 1, &ub->buffer);
     }
}

void GraphicsManagerD3D11::UniformBufferUpdate(UniformBuffer *uniformBuffer, void *data)
{
     UniformBufferD3D11 *ub = (UniformBufferD3D11 *)uniformBuffer;
     D3D11_MAPPED_SUBRESOURCE mappedSubresourece;
     if(FAILED(deviceContext->Map(ub->buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubresourece)))
     {
          ASSERT(!"Error mapping const buffer");
     }
     memcpy(mappedSubresourece.pData, data, ub->dataSize);
     deviceContext->Unmap(ub->buffer, 0);
}


VertexShader* GraphicsManagerD3D11::VertexShaderAlloc(File file)
{
     VertexShaderD3D11 *sh = vertexShaderAllocator.Alloc();

     ID3DBlob *errorShader;
     ID3DBlob *vertShaderCompiled;
     // Compile vertex Shader
     D3DCompile(file.data, file.size,
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
          device->CreateVertexShader(
               vertShaderCompiled->GetBufferPointer(),
               vertShaderCompiled->GetBufferSize(), 0,
               &sh->vertexShader);
     }
     // Create the input layout for this shader
     if(FAILED(CreateInputLayoutDescFromVertexShaderSignature(vertShaderCompiled, device, &sh->layout)))
     {
          ASSERT(!"Error creating input layout for this shader");
     }
     vertShaderCompiled->Release();
     return (VertexShader *)sh;
}

void GraphicsManagerD3D11::VertexShaderFree(VertexShader *shader)
{
     VertexShaderD3D11 *sh = (VertexShaderD3D11 *)shader;
     sh->layout->Release();
     sh->vertexShader->Release();
     vertexShaderAllocator.Free(sh);

}

void GraphicsManagerD3D11::VertexShaderBind(VertexShader *shader)
{
    VertexShaderD3D11*sh = (VertexShaderD3D11*)shader;
     deviceContext->IASetInputLayout(sh->layout);
     deviceContext->VSSetShader(sh->vertexShader, 0, 0);
}

FragmentShader *GraphicsManagerD3D11::FragmentShaderAlloc(File file)
{
    FragmentShaderD3D11* sh = fragmentShaderAllocator.Alloc();
    ID3DBlob* errorShader;
    ID3DBlob *fragShaderCompiled;
    // Compile fragment Shader
    D3DCompile(file.data, file.size,
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
        device->CreatePixelShader(
            fragShaderCompiled->GetBufferPointer(),
            fragShaderCompiled->GetBufferSize(), 0,
            &sh->pixelShader);
    }
    fragShaderCompiled->Release();
    return (FragmentShader *)sh;
}

void GraphicsManagerD3D11::FragmentShaderFree(FragmentShader* shader)
{
    FragmentShaderD3D11* sh = (FragmentShaderD3D11*)shader;
    sh->pixelShader->Release();
    fragmentShaderAllocator.Free(sh);
}

void GraphicsManagerD3D11::FragmentShaderBind(FragmentShader* shader)
{
    FragmentShaderD3D11* sh = (FragmentShaderD3D11*)shader;
    deviceContext->PSSetShader(sh->pixelShader, 0, 0);
}

Texture *GraphicsManagerD3D11::TextureAlloc(const char *filepath)
{
     TextureD3D11 *tx = textureAllocator.Alloc();

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

     if (FAILED(device->CreateTexture2D(&texDesc, 0, &tx->texture)))
     {
          ASSERT(!"Error creating texture");
     }

     if (tx->texture)
     {
         deviceContext->UpdateSubresource(tx->texture, 0, 0, subresourceData.pSysMem, subresourceData.SysMemPitch, 0);
         D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
         srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
         srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
         srvDesc.Texture2D.MipLevels = -1;
         srvDesc.Texture2D.MostDetailedMip = 0;
         if (FAILED(device->CreateShaderResourceView(tx->texture, &srvDesc, &tx->shaderResourceView)))
         {
             ASSERT(!"Error creating texture shader resource view");
         }
         if (tx->shaderResourceView)
         {
             deviceContext->GenerateMips(tx->shaderResourceView);
         }
     }
     stbi_image_free(data);
     return (Texture *)tx;
}

void GraphicsManagerD3D11::TextureFree(Texture *texture)
{
     TextureD3D11 *tx = (TextureD3D11 *)texture;
     tx->texture->Release();
     tx->shaderResourceView->Release();
     textureAllocator.Free(tx);
}

void GraphicsManagerD3D11::TextureBind(Texture *texture, i32 slot)
{
     TextureD3D11 *tx = (TextureD3D11 *)texture;
     deviceContext->PSSetShaderResources(slot, 1, &tx->shaderResourceView);
}

i32 GraphicsManagerD3D11::TextureWidth(Texture *texture)
{
     TextureD3D11 *tx = (TextureD3D11 *)texture;
     return tx->w;
}

i32 GraphicsManagerD3D11::TextureHeight(Texture *texture)
{
     TextureD3D11 *tx = (TextureD3D11 *)texture;
     return tx->h;
}

// TODO: use macros for debug build
void GraphicsManagerD3D11::DebugInit()
{
#if ANT_DEBUG
     debugRenderer.Init(device);
#endif
}

void GraphicsManagerD3D11::DebugTerminate()
{
#if ANT_DEBUG
     debugRenderer.Terminate();
#endif
}

void GraphicsManagerD3D11::DebugPresent()
{
#if ANT_DEBUG
     debugRenderer.Present(deviceContext);
#endif
}
     
void GraphicsManagerD3D11::DebugDrawLine(const Vector3& a, const Vector3& b, const Vector3& color)
{
#if ANT_DEBUG
     debugRenderer.DrawLine(deviceContext, a, b, color);
#endif
}

void GraphicsManagerD3D11::DebugDrawSphere(const Vector3& c, f32 r, i32 hSlice, i32 vSlice, const Vector3& color)
{
#if ANT_DEBUG
     Vector3 up = Vector3(0.0f, 1.0f, 0.0f);
     Vector3 right = Vector3(1.0f, 0.0f, 0.0f);
     // TODO: try to make it fit perfectly

     // Draw the vertical Lines
     f32 hInc = ANT_PI / (f32)hSlice;
     f32 vInc = (2.0f * ANT_PI) / (f32)vSlice;
     for(i32 j = 0; j < hSlice; ++j)
     {
          Vector3 dir = up;
          for(i32 i = 0; i < vSlice; ++i)
          {
               Vector3 a = dir * r;
               dir = Quaternion::AngleAxis(vInc, right) * dir;
               Vector3 b = dir * r;
               GraphicsManager::Get()->DebugDrawLine(c + a, c + b, color);
          }
          right = Quaternion::AngleAxis(hInc, up) * right;
     }

     // Draw the horizontal lines
     hInc = (2.0f * ANT_PI) / (f32)hSlice;
     vInc = ANT_PI / (f32)vSlice;
     right = Vector3(1.0f, 0.0f, 0.0f);
     Vector3 dir = up;
     for(i32 j = 0; j < vSlice - 2; ++j)
     {
          dir = Quaternion::AngleAxis(vInc, right) * dir;
          for(i32 i = 0; i < hSlice; ++i)
          {
               Vector3 a = dir * r;
               dir = Quaternion::AngleAxis(hInc, up) * dir;
               Vector3 b = dir * r;
               GraphicsManager::Get()->DebugDrawLine(c + a, c + b, color);
          }
     }

#endif
}

void GraphicsManagerD3D11::DebugDrawCube(const Vector3& c, const Vector3& hExtend, const Vector3& color)
{
#if ANT_DEBUG
     // TODO: ...
#endif
}


// Private member functions
void GraphicsManagerD3D11::CreateDeviceAndSwapChain()
{
     i32 deviceFlags = 0;
#if ANT_DEBUG
     printf("Directx11 debug mode set\n");
     deviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

     D3D_DRIVER_TYPE driverTypes[] = {
          D3D_DRIVER_TYPE_HARDWARE,
          D3D_DRIVER_TYPE_WARP,
          D3D_DRIVER_TYPE_SOFTWARE
     };
     D3D_FEATURE_LEVEL featureLevels[] = {
          D3D_FEATURE_LEVEL_11_1,
          D3D_FEATURE_LEVEL_11_0,
          D3D_FEATURE_LEVEL_10_1,
          D3D_FEATURE_LEVEL_10_0
     };
     
     u32 driverTypesCount = ARRAY_LENGTH(driverTypes);
     u32 featureLevelsCount = ARRAY_LENGTH(featureLevels);

     HRESULT hr = S_OK;
     D3D_FEATURE_LEVEL featureLevel;
     for(u32 driver = 0; driver < driverTypesCount; ++driver)
     {
               hr = D3D11CreateDevice(0, driverTypes[driver], 0, deviceFlags,
                    featureLevels, featureLevelsCount,
                    D3D11_SDK_VERSION, &device, &featureLevel, &deviceContext);
               if (hr == E_INVALIDARG)
               {
                   // DirectX 11.0 platforms will not recognize D3D_FEATURE_LEVEL_11_1 so we need to retry without it
                    hr = D3D11CreateDevice(0, driverTypes[driver], 0, deviceFlags,
                         &featureLevels[1], featureLevelsCount - 1,
                         D3D11_SDK_VERSION, &device, &featureLevel, &deviceContext);
               }

          if(SUCCEEDED(hr))
          {
               break;
          }
     }
     if(FAILED(hr))
     {
          ASSERT(!"Error creating D3D11 Device!");
     }

     // check for msaa
     u32 msaaQuality4x;
     device->CheckMultisampleQualityLevels(DXGI_FORMAT_R8G8B8A8_UNORM, 4, &msaaQuality4x);
     if (msaaQuality4x <= 0)
     {
          ASSERT(!"Error msaa 4x not supported.");
     }

    // Obtain DXGI factory from device (since we used nullptr for pAdapter above)
     IDXGIFactory1* dxgiFactory = nullptr;
     {
          IDXGIDevice* dxgiDevice = nullptr;
          hr = device->QueryInterface( __uuidof(IDXGIDevice), reinterpret_cast<void**>(&dxgiDevice) );
          if (SUCCEEDED(hr))
          {
               IDXGIAdapter* adapter = nullptr;
               hr = dxgiDevice->GetAdapter(&adapter);
               if (SUCCEEDED(hr))
               {
                    hr = adapter->GetParent( __uuidof(IDXGIFactory1), reinterpret_cast<void**>(&dxgiFactory) );
                    adapter->Release();
               }
               dxgiDevice->Release();
          }
     }
     if (FAILED(hr))
     {
          ASSERT(!"Error getting DXGIFactory");
     }    

    // Create swap chain
    IDXGIFactory2* dxgiFactory2 = nullptr;
    hr = dxgiFactory->QueryInterface( __uuidof(IDXGIFactory2), reinterpret_cast<void**>(&dxgiFactory2) );
    if (dxgiFactory2)
    {
        // DirectX 11.1 or later
        hr = device->QueryInterface( __uuidof(ID3D11Device1), reinterpret_cast<void**>(&device1) );
        if (SUCCEEDED(hr))
        {
            (void) deviceContext->QueryInterface( __uuidof(ID3D11DeviceContext1), reinterpret_cast<void**>(&deviceContext1) );
        }

        DXGI_SWAP_CHAIN_DESC1 sd = {};
        sd.Width = windowWidth;
        sd.Height = windowHeight;
        sd.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        sd.SampleDesc.Count = 1;
        sd.SampleDesc.Quality = 0;
        sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        sd.BufferCount = 2;

        hr = dxgiFactory2->CreateSwapChainForHwnd( device, *window, &sd, nullptr, nullptr, &swapChain1 );
        if (SUCCEEDED(hr))
        {
            hr = swapChain1->QueryInterface( __uuidof(IDXGISwapChain), reinterpret_cast<void**>(&swapChain) );
        }
        dxgiFactory2->Release();
    }
    else
    {
        // DirectX 11.0 systems
        DXGI_SWAP_CHAIN_DESC sd = {};
        sd.BufferCount = 2;
        sd.BufferDesc.Width = windowWidth;
        sd.BufferDesc.Height = windowHeight;
        sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        sd.BufferDesc.RefreshRate.Numerator = 60;
        sd.BufferDesc.RefreshRate.Denominator = 1;
        sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        sd.OutputWindow = *window;
        sd.SampleDesc.Count = 1;
        sd.SampleDesc.Quality = 0;
        sd.Windowed = TRUE;

        hr = dxgiFactory->CreateSwapChain( device, &sd, &swapChain );
    }
     
    dxgiFactory->Release();
}


void GraphicsManagerD3D11::CreateRenderTargetView()
{
     // create render target view
     ID3D11Texture2D *backBufferTexture;
     swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void **)&backBufferTexture);
     if (backBufferTexture && FAILED(device->CreateRenderTargetView(backBufferTexture, 0, &renderTargetView)))
     {
          ASSERT(!"Error creating render target.");
     }
     backBufferTexture->Release();
}

void GraphicsManagerD3D11::CreateDepthStencilView(i32 width, i32 height)
{     
     // create the depth stencil texture
     ID3D11Texture2D *depthStencilTexture;
     D3D11_TEXTURE2D_DESC depthStencilTextureDesc;
     depthStencilTextureDesc.Width = width;
     depthStencilTextureDesc.Height = height;
     depthStencilTextureDesc.MipLevels = 1;
     depthStencilTextureDesc.ArraySize = 1;
     depthStencilTextureDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
     depthStencilTextureDesc.SampleDesc.Count = 1;
     depthStencilTextureDesc.SampleDesc.Quality = 0;
     depthStencilTextureDesc.Usage = D3D11_USAGE_DEFAULT;
     depthStencilTextureDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
     depthStencilTextureDesc.CPUAccessFlags = 0;
     depthStencilTextureDesc.MiscFlags = 0;
     if (FAILED(device->CreateTexture2D(&depthStencilTextureDesc, 0, &depthStencilTexture)))
     {
          ASSERT(!"Error creating depth stencil texture.");
     }
     if (depthStencilTexture && FAILED(device->CreateDepthStencilView(depthStencilTexture, 0, &depthStencilView)))
     {
          ASSERT(!"Error creating depth stencil view.");
     }
     depthStencilTexture->Release();
}

HRESULT GraphicsManagerD3D11::CreateInputLayoutDescFromVertexShaderSignature(ID3DBlob* pShaderBlob, ID3D11Device* pD3DDevice, ID3D11InputLayout** pInputLayout )
{
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
     Frame frame = MemoryManager::Get()->GetFrame(SCRATCH_MEMORY);

     Array<D3D11_INPUT_ELEMENT_DESC> inputLayoutDesc;
     inputLayoutDesc.Init(shaderDesc.InputParameters, SCRATCH_MEMORY);
     
     for (u32 i = 0; i < shaderDesc.InputParameters; i++ )
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
     HRESULT hr = device->CreateInputLayout(inputLayoutDesc.data, inputLayoutDesc.size, pShaderBlob->GetBufferPointer(), pShaderBlob->GetBufferSize(), pInputLayout);

     MemoryManager::Get()->ReleaseFrame(frame);
     
     //Free allocation shader reflection memory
     pVertexShaderReflection->Release();
     return hr;
}

void GraphicsManagerD3D11::CreateSamplerStates()
{
     D3D11_SAMPLER_DESC colorMapDesc = {};
     colorMapDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
     colorMapDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
     colorMapDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
     colorMapDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
     colorMapDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
     colorMapDesc.MaxLOD = D3D11_FLOAT32_MAX;
     if (FAILED(device->CreateSamplerState(&colorMapDesc, &samplerStateLinearClamp)))
     {
          ASSERT(!"Error: Failed Creating sampler state Point\n");
     }
     colorMapDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
     colorMapDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
     colorMapDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
     if (FAILED(device->CreateSamplerState(&colorMapDesc, &samplerStateLinearWrap)))
     {
          ASSERT(!"Error: Failed Creating sampler state Linear\n");
     }
}

void GraphicsManagerD3D11::CreateRasterizerStates()
{
     D3D11_RASTERIZER_DESC fillRasterizerFrontDesc;
     ZeroMemory(&fillRasterizerFrontDesc, sizeof(fillRasterizerFrontDesc));
     fillRasterizerFrontDesc.FillMode = D3D11_FILL_SOLID;
     fillRasterizerFrontDesc.CullMode = D3D11_CULL_FRONT;
     fillRasterizerFrontDesc.FrontCounterClockwise = false;
     fillRasterizerFrontDesc.DepthClipEnable = true;
     fillRasterizerFrontDesc.AntialiasedLineEnable = true;
     fillRasterizerFrontDesc.MultisampleEnable = true;
     device->CreateRasterizerState(&fillRasterizerFrontDesc, &fillRasterizerCullFront);

     D3D11_RASTERIZER_DESC fillRasterizerBackDesc;
     ZeroMemory(&fillRasterizerBackDesc, sizeof(fillRasterizerBackDesc));
     fillRasterizerBackDesc.FillMode = D3D11_FILL_SOLID;
     fillRasterizerBackDesc.CullMode = D3D11_CULL_BACK;
     fillRasterizerBackDesc.FrontCounterClockwise = false;
     fillRasterizerBackDesc.DepthClipEnable = true;
     fillRasterizerBackDesc.AntialiasedLineEnable = true;
     fillRasterizerBackDesc.MultisampleEnable = true;
     device->CreateRasterizerState(&fillRasterizerBackDesc, &fillRasterizerCullBack);

     D3D11_RASTERIZER_DESC fillRasterizerNoneDesc;
     ZeroMemory(&fillRasterizerNoneDesc, sizeof(fillRasterizerNoneDesc));
     fillRasterizerNoneDesc.FillMode = D3D11_FILL_SOLID;
     fillRasterizerNoneDesc.CullMode = D3D11_CULL_NONE;
     fillRasterizerNoneDesc.FrontCounterClockwise = false;
     fillRasterizerNoneDesc.DepthClipEnable = true;
     fillRasterizerNoneDesc.AntialiasedLineEnable = true;
     fillRasterizerNoneDesc.MultisampleEnable = true;
     device->CreateRasterizerState(&fillRasterizerNoneDesc, &fillRasterizerCullNone);

     D3D11_RASTERIZER_DESC wireFrameRasterizerDesc;
     ZeroMemory(&wireFrameRasterizerDesc, sizeof(wireFrameRasterizerDesc));
     wireFrameRasterizerDesc.FillMode = D3D11_FILL_WIREFRAME;
     wireFrameRasterizerDesc.CullMode = D3D11_CULL_NONE;
     wireFrameRasterizerDesc.FrontCounterClockwise = false;
     wireFrameRasterizerDesc.DepthClipEnable = true;
     wireFrameRasterizerDesc.AntialiasedLineEnable = true;
     wireFrameRasterizerDesc.MultisampleEnable = true;
     device->CreateRasterizerState(&wireFrameRasterizerDesc, &wireFrameRasterizer);
}

