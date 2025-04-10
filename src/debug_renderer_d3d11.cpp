void DebugRenderer::Init()
{
     GraphicsManagerState *state = &gGraphicsManagerState;

     bufferUsed = 0;
     bufferSize = 10000;

     cpuBuffer = (VertexLine *)MemoryManager::Get()->Alloc(sizeof(VertexLine) * bufferSize, STATIC_MEMORY);

     D3D11_BUFFER_DESC vertexDesc;
     ZeroMemory(&vertexDesc, sizeof(vertexDesc));
     vertexDesc.Usage = D3D11_USAGE_DYNAMIC;
     vertexDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
     vertexDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
     vertexDesc.ByteWidth = sizeof(VertexLine) * bufferSize;
     if(FAILED(state->device->CreateBuffer(&vertexDesc, 0, &gpuBuffer)))
     {
          ASSERT(!"Error create line renderer GPU buffer.");
     }
     
     Frame frame = MemoryManager::Get()->GetFrame();
     // Load the shader
     File vertFile = PlatformReadFile("../data/shaders/line_vert.hlsl", FRAME_MEMORY);
     File fragFile = PlatformReadFile("../data/shaders/line_frag.hlsl", FRAME_MEMORY);
     shader = GraphicsManager::Get()->ShaderAlloc(vertFile, fragFile);

     MemoryManager::Get()->ReleaseFrame(frame);

     printf("Debug Renderer Init!\n");
     
}

void DebugRenderer::Terminate()
{
     gpuBuffer->Release();
     GraphicsManager::Get()->ShaderFree(shader);

     printf("Debug Renderer Terminate!\n");
}

void DebugRenderer::Present()
{
     GraphicsManagerState *state = &gGraphicsManagerState;

     D3D11_MAPPED_SUBRESOURCE bufferData;
     ZeroMemory(&bufferData, sizeof(bufferData));
     state->deviceContext->Map(gpuBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &bufferData);
     memcpy(bufferData.pData, cpuBuffer, sizeof(VertexLine)*bufferUsed);
     state->deviceContext->Unmap(gpuBuffer, 0);

     GraphicsManager::Get()->ShaderBind(shader);
     
     u32 stride = sizeof(VertexLine);
     u32 offset = 0;
     state->deviceContext->IASetVertexBuffers(0, 1, &gpuBuffer, &stride, &offset);
     state->deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
     state->deviceContext->Draw(bufferUsed, 0);
     bufferUsed = 0;
}


void DebugRenderer::DrawLine(vec3& a, vec3 &b)
{
     GraphicsManagerState *state = &gGraphicsManagerState;
     VertexLine line[2] = {};
     line[0].pos = a;
     line[0].col = vec3(0.0f, 1.0f, 0.0f);
     line[1].pos = b;
     line[1].col = vec3(0.0f, 1.0f, 0.0f);

     if(bufferUsed + 2 > bufferSize)
     {
          Present();
     }
     
     ASSERT(bufferUsed + 2 <= bufferSize);
     VertexLine *vertex = cpuBuffer + bufferUsed;
     memcpy(vertex, line, sizeof(VertexLine)*2);
     bufferUsed += 2;
}
