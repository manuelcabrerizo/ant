#include <graphics_manager.h>
#include <graphics_manager_d3d11.h>
#include <memory_manager.h>
#include <new>

GraphicsManager *GraphicsManager::instance = nullptr;

void GraphicsManager::Init(void *osWindow, i32 width, i32 height, GraphicsManagerType type, i32 stackNum)
{
     if(instance != nullptr)
     {
          ASSERT(!"Error graphics manager already initialize");
     }
     
     switch(type)
     {
     case GraphicsManagerType::D3D11:
     {
          void *buffer = MemoryManager::Get()->Alloc(sizeof(GraphicsManagerD3D11), stackNum);
          instance = new (buffer) GraphicsManagerD3D11;
     }break;
     case GraphicsManagerType::D3D12:
     {
          // TODO: ...
     }break;
     case GraphicsManagerType::OPENGL:
     {
          // TODO: ...
     }break;
     case GraphicsManagerType::METAL:
     {
          // TODO: ...
     }break;
     case GraphicsManagerType::VULKAN:
     {
          // TODO: ...
     }break;
     }

     ASSERT(instance != nullptr);

     instance->Initialize(osWindow, width, height, stackNum);
}

void GraphicsManager::Terminate()
{
     if(instance == nullptr)
     {
          ASSERT(!"Error: graphics manager has not been initialize");
     }

     instance->Shutdown();
}

GraphicsManager *GraphicsManager::Get()
{
     return instance;
}
