
// Public Interface

void GraphicsManagerD3D11::Initialize(void *osWindow, i32 width, i32 height, i32 stackNum)
{    
     window = (HWND *)osWindow;
     windowWidth = width;
     windowHeight = height;
     
     CreateDeviceAndSwapChain();
     CreateRenderTargetView();
     CreateDepthStencilView();
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
     shaderAllocator.Init(stackNum);
     textureAllocator.Init(stackNum);

     printf("DirectX11 Initialized!\n");

     SetRasterizerStateCullBack();
}

void GraphicsManagerD3D11::Shutdown()
{
     deviceContext->Flush();
     
     wireFrameRasterizer->Release();
     fillRasterizerCullBack->Release();
     fillRasterizerCullFront->Release();
     fillRasterizerCullNone->Release();
     
     samplerStateLinearClamp->Release();
     samplerStateLinearWrap->Release();
     
     depthStencilView->Release();
     renderTargetView->Release();
     swapChain2->Release();
     swapChain->Release();
     deviceContext->Release();
     device->Release();

     printf("DirectX11 Terminated!\n");
}

void GraphicsManagerD3D11::BeginFrame(f32 r, f32 g, f32 b)
{
     f32 clearColor[] = { r, g, b, 1.0f };
     deviceContext->ClearRenderTargetView(renderTargetView, clearColor);
     deviceContext->ClearDepthStencilView(depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
}

void GraphicsManagerD3D11::EndFrame(i32 vsync)
{
     /*
#if ANT_DEBUG
     debugRenderer.Present();
#endif
     */
     if(vsync > 0)
     {
          WaitForSingleObject(waitHandle, INFINITE); // blocks until frame is ready
     }
     swapChain->Present(vsync, 0);
     deviceContext->OMSetRenderTargets(1, &renderTargetView, depthStencilView);

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


Shader *GraphicsManagerD3D11::ShaderAlloc(File vertFile, File fragFile)
{
     ShaderD3D11 *sh = shaderAllocator.Alloc();

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
          device->CreatePixelShader(
               fragShaderCompiled->GetBufferPointer(),
               fragShaderCompiled->GetBufferSize(), 0,
               &sh->pixelShader);
     }

     vertShaderCompiled->Release();
     fragShaderCompiled->Release();

     return (Shader *)sh;
}

void GraphicsManagerD3D11::ShaderFree(Shader *shader)
{
     ShaderD3D11 *sh = (ShaderD3D11 *)shader;
     sh->layout->Release();
     sh->vertexShader->Release();
     sh->pixelShader->Release();
     shaderAllocator.Free(sh);

}

void GraphicsManagerD3D11::ShaderBind(Shader *shader)
{
     ShaderD3D11 *sh = (ShaderD3D11 *)shader;
     deviceContext->IASetInputLayout(sh->layout);
     deviceContext->VSSetShader(sh->vertexShader, 0, 0);
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

     deviceContext->GenerateMips(tx->shaderResourceView);

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

/*
void GraphicsManager::DebugInit()
{
#if ANT_DEBUG
     GraphicsManagerState *state = &gGraphicsManagerState;
     state->debugRenderer.Init();
#endif
}

void GraphicsManager::DebugTerminate()
{
#if ANT_DEBUG
     GraphicsManagerState *state = &gGraphicsManagerState;
     state->debugRenderer.Terminate();
#endif
}

void GraphicsManager::DebugPresent()
{
#if ANT_DEBUG
     GraphicsManagerState *state = &gGraphicsManagerState;
     state->debugRenderer.Present();
#endif
}
     
void GraphicsManager::DebugDrawLine(vec3& a, vec3& b)
{
#if ANT_DEBUG
     GraphicsManagerState *state = &gGraphicsManagerState;
     state->debugRenderer.DrawLine(a, b);
#endif
}


void GraphicsManager::DebugDrawSphere(vec3& c, f32 r, i32 hSlice, i32 vSlice)
{
#if ANT_DEBUG
     vec3 up = vec3(0.0f, 1.0f, 0.0f);
     vec3 right = vec3(1.0f, 0.0f, 0.0f);
     // TODO: try to make it fit perfectly

     // Draw the vertical Lines
     f32 hInc = ANT_PI / (f32)hSlice;
     f32 vInc = (2.0f * ANT_PI) / (f32)vSlice;
     for(i32 j = 0; j < hSlice; ++j)
     {
          vec3 dir = up;
          for(i32 i = 0; i < vSlice; ++i)
          {
               vec3 a = dir * r;
               dir = mat3(rotate(mat4(1.0f), vInc, right)) * dir;
               vec3 b = dir * r;
               GraphicsManager::Get()->DebugDrawLine(c + a, c + b);
          }
          right = mat3(rotate(mat4(1.0f), hInc, up)) * right;
     }

     // Draw the horizontal lines
     hInc = (2.0f * ANT_PI) / (f32)hSlice;
     vInc = ANT_PI / (f32)vSlice;
     right = vec3(1.0f, 0.0f, 0.0f);
     vec3 dir = up;
     for(i32 j = 0; j < vSlice - 2; ++j)
     {
          dir = mat3(rotate(mat4(1.0f), vInc, right)) * dir;
          for(i32 i = 0; i < hSlice; ++i)
          {
               vec3 a = dir * r;
               dir = mat3(rotate(mat4(1.0f), hInc, up)) * dir;
               vec3 b = dir * r;
               GraphicsManager::Get()->DebugDrawLine(c + a, c + b);
          }
     }
#endif
}

void GraphicsManager::DebugDrawCube(vec3& c, vec3& hExtend)
{
#if ANT_DEBUG
#endif
}
*/

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
          D3D_FEATURE_LEVEL_11_0,
          D3D_FEATURE_LEVEL_10_1,
          D3D_FEATURE_LEVEL_10_0
     };
     
     i32 driverTypesCount = ARRAY_LENGTH(driverTypes);
     i32 featureLevelsCount = ARRAY_LENGTH(featureLevels);

     // Select the best gpu   
     IDXGIFactory1 * pFactory;
     HRESULT hr = CreateDXGIFactory1(__uuidof(IDXGIFactory1), (void**)(&pFactory));
          
     UINT i = 0; 
     IDXGIAdapter *pSelectedAdapter = nullptr;
     IDXGIAdapter * pAdapter = nullptr; 
     while(pFactory->EnumAdapters(i, &pAdapter) != DXGI_ERROR_NOT_FOUND) 
     { 
          DXGI_ADAPTER_DESC desc;
          pAdapter->GetDesc(&desc);
          if (desc.DedicatedVideoMemory > 0)
          {
               printf("Dedicated adapter found: %ls\n", desc.Description);
               pSelectedAdapter = pAdapter;
               break;
          }
          ++i;
     }

     D3D_FEATURE_LEVEL featureLevel;
     D3D_DRIVER_TYPE driverType;
     if(pSelectedAdapter)
     {
          if (FAILED(D3D11CreateDevice(pSelectedAdapter, D3D_DRIVER_TYPE_UNKNOWN, 0, deviceFlags,
               featureLevels, featureLevelsCount,
               D3D11_SDK_VERSION, &device, &featureLevel, &deviceContext)))
          {
              ASSERT("Error Creating Directx11 device");
          }
     }
     else
     {
          bool driverSelected = false;
          for(u32 driver = 0; driver < driverTypesCount; ++driver)
          {
               HRESULT result = D3D11CreateDevice(0, driverTypes[driver], 0, deviceFlags,
                    featureLevels, featureLevelsCount,
                    D3D11_SDK_VERSION, &device, &featureLevel, &deviceContext);
               if(SUCCEEDED(result))
               {
                    driverType = driverTypes[driver];
                    printf("Driver selected %d\n", driver);
                    driverSelected = true;
                    break;
               }
          }
          ASSERT(driverSelected);
     }

     IDXGIDevice *dxgiDevice;
     IDXGIAdapter *dxgiAdapter;
     IDXGIFactory2 *dxgiFactory;
     device->QueryInterface(__uuidof(IDXGIDevice), (void **)&dxgiDevice);
     dxgiDevice->GetParent(__uuidof(IDXGIAdapter), (void **)&dxgiAdapter);
     dxgiAdapter->GetParent(__uuidof(IDXGIFactory2), (void **)&dxgiFactory);

     // DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL and DXGI_SWAP_EFFECT_FLIP_DISCARD
     // TODO: this is for windows 11 and wwe need to fix fullscreen
     // TODO: After calling SetFullscreenState, the app must call ResizeBuffers before Present.
     DXGI_SWAP_CHAIN_DESC1 swapChainDesc;
     ZeroMemory(&swapChainDesc, sizeof(swapChainDesc));
     swapChainDesc.Width = windowWidth;
     swapChainDesc.Height = windowHeight;
     swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
     swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
     swapChainDesc.BufferCount = 2;
     swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
     swapChainDesc.SampleDesc.Count = 1;
     swapChainDesc.SampleDesc.Quality = 0;
     swapChainDesc.Scaling = DXGI_SCALING_STRETCH;
     swapChainDesc.AlphaMode = DXGI_ALPHA_MODE_IGNORE;
     swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH | DXGI_SWAP_CHAIN_FLAG_FRAME_LATENCY_WAITABLE_OBJECT;
     if (FAILED(dxgiFactory->CreateSwapChainForHwnd(device, *window, &swapChainDesc, nullptr, nullptr, &swapChain)))
     {
         ASSERT(!"Error creating swap chain.");
     }

     swapChain->QueryInterface(__uuidof(IDXGISwapChain2), (void **)&swapChain2);
     waitHandle = swapChain2->GetFrameLatencyWaitableObject();
     swapChain2->SetMaximumFrameLatency(1);

     dxgiDevice->Release();
     dxgiAdapter->Release();
     dxgiFactory->Release();
}


void GraphicsManagerD3D11::CreateRenderTargetView()
{
     // create render target view
     ID3D11Texture2D *backBufferTexture;
     swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void **)&backBufferTexture);
     if (FAILED(device->CreateRenderTargetView(backBufferTexture, 0, &renderTargetView)))
     {
          ASSERT(!"Error creating render target.");
     }
     backBufferTexture->Release();
}

void GraphicsManagerD3D11::CreateDepthStencilView()
{
     // TODO: remove this check, it does not belong to this function
     
     // check for msaa
     u32 msaaQuality4x;
     device->CheckMultisampleQualityLevels(DXGI_FORMAT_R8G8B8A8_UNORM, 4, &msaaQuality4x);
     if (msaaQuality4x <= 0)
     {
          ASSERT(!"Error msaa 4x not supported.");
     }
     
     // create the depth stencil texture
     ID3D11Texture2D *depthStencilTexture;
     D3D11_TEXTURE2D_DESC depthStencilTextureDesc;
     depthStencilTextureDesc.Width = windowWidth;
     depthStencilTextureDesc.Height = windowHeight;
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
     if (FAILED(device->CreateDepthStencilView(depthStencilTexture, 0, &depthStencilView)))
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
     Frame frame = MemoryManager::Get()->GetFrame();

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
     fillRasterizerFrontDesc.FrontCounterClockwise = true;
     fillRasterizerFrontDesc.DepthClipEnable = true;
     fillRasterizerFrontDesc.AntialiasedLineEnable = true;
     fillRasterizerFrontDesc.MultisampleEnable = true;
     device->CreateRasterizerState(&fillRasterizerFrontDesc, &fillRasterizerCullFront);

     D3D11_RASTERIZER_DESC fillRasterizerBackDesc;
     ZeroMemory(&fillRasterizerBackDesc, sizeof(fillRasterizerBackDesc));
     fillRasterizerBackDesc.FillMode = D3D11_FILL_SOLID;
     fillRasterizerBackDesc.CullMode = D3D11_CULL_BACK;
     fillRasterizerBackDesc.FrontCounterClockwise = true;
     fillRasterizerBackDesc.DepthClipEnable = true;
     fillRasterizerBackDesc.AntialiasedLineEnable = true;
     fillRasterizerBackDesc.MultisampleEnable = true;
     device->CreateRasterizerState(&fillRasterizerBackDesc, &fillRasterizerCullBack);

     D3D11_RASTERIZER_DESC fillRasterizerNoneDesc;
     ZeroMemory(&fillRasterizerNoneDesc, sizeof(fillRasterizerNoneDesc));
     fillRasterizerNoneDesc.FillMode = D3D11_FILL_SOLID;
     fillRasterizerNoneDesc.CullMode = D3D11_CULL_NONE;
     fillRasterizerNoneDesc.FrontCounterClockwise = true;
     fillRasterizerNoneDesc.DepthClipEnable = true;
     fillRasterizerNoneDesc.AntialiasedLineEnable = true;
     fillRasterizerNoneDesc.MultisampleEnable = true;
     device->CreateRasterizerState(&fillRasterizerNoneDesc, &fillRasterizerCullNone);

     D3D11_RASTERIZER_DESC wireFrameRasterizerDesc;
     ZeroMemory(&wireFrameRasterizerDesc, sizeof(wireFrameRasterizerDesc));
     wireFrameRasterizerDesc.FillMode = D3D11_FILL_WIREFRAME;
     wireFrameRasterizerDesc.CullMode = D3D11_CULL_NONE;
     wireFrameRasterizerDesc.FrontCounterClockwise = true;
     wireFrameRasterizerDesc.DepthClipEnable = true;
     wireFrameRasterizerDesc.AntialiasedLineEnable = true;
     wireFrameRasterizerDesc.MultisampleEnable = true;
     device->CreateRasterizerState(&wireFrameRasterizerDesc, &wireFrameRasterizer);
}

