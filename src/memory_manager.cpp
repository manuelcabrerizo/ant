#include <memory_manager.h>
#include <memory.h>

MemoryManager MemoryManager::instance;
bool MemoryManager::initialize = false;

void MemoryManager::Init(u64 staticMemorySize, u64 frameMemorySize, u64 scratchMemorySize, size_t align)
{
     if(initialize)
     {
          ASSERT(!"Error: memory manager already initialize");
     }
     memset(&instance, 0, sizeof(MemoryManager));
     instance.allocator.Init(frameMemorySize + scratchMemorySize, align);
     instance.staticAllocator.Init(staticMemorySize, align);
     initialize = true;
}

void MemoryManager::Terminate()
{
     if(!initialize)
     {
          ASSERT(!"Error: memory manager has not been initialize");
     }
     instance.allocator.Terminate();
     instance.staticAllocator.Terminate();
     initialize = false;
}

MemoryManager *MemoryManager::Get()
{
     if(!initialize)
     {
          ASSERT(!"Error: memory manager has not been initialize");
     }
     return &instance;
}

void *MemoryManager::Alloc(u64 size, i32 memoryType)
{
    switch (memoryType)
    {
    case FRAME_MEMORY:
    case SCRATCH_MEMORY:
        return allocator.Alloc(size, memoryType);
    case STATIC_MEMORY:
        return staticAllocator.Alloc(size);
    }
}

Frame MemoryManager::GetFrame(i32 memoryType)
{
    switch (memoryType)
    {
    case FRAME_MEMORY:
    case SCRATCH_MEMORY:
        return allocator.GetFrame(memoryType);
    case STATIC_MEMORY: 
        ASSERT(!"ERROR: STATIC MEMORY CANT BY FRAMED");
    }
}

void MemoryManager::ReleaseFrame(Frame frame)
{
    allocator.ReleaseFrame(frame);
}

