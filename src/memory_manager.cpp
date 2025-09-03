#include <memory_manager.h>
#include <memory.h>

MemoryManager MemoryManager::instance;
bool MemoryManager::initialize = false;

void MemoryManager::Init(u64 staticMemorySize, u64 frameMemorySize, u64 scratchMemorySize)
{
     if(initialize)
     {
          ASSERT(!"Error: memory manager already initialize");
     }
     memset(&instance, 0, sizeof(MemoryManager));
     instance.staticAllocator.Init(staticMemorySize);
     instance.frameAllocator.Init(frameMemorySize);
     instance.scratchAllocator.Init(scratchMemorySize);
     initialize = true;
}

void MemoryManager::Terminate()
{
     if(!initialize)
     {
          ASSERT(!"Error: memory manager has not been initialize");
     }
     instance.scratchAllocator.Terminate();
     instance.frameAllocator.Terminate();
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
        return frameAllocator.Alloc(size);
    case SCRATCH_MEMORY:
        return scratchAllocator.Alloc(size);
    case STATIC_MEMORY:
        return staticAllocator.Alloc(size);
    }
}

Frame MemoryManager::GetFrame(i32 memoryType)
{
    switch (memoryType)
    {
    case FRAME_MEMORY:
        return frameAllocator.GetFrame(memoryType);
    case SCRATCH_MEMORY:
        return scratchAllocator.GetFrame(memoryType);
    case STATIC_MEMORY: 
        ASSERT(!"ERROR: STATIC MEMORY CANT BY FRAMED");
    }
}

void MemoryManager::ReleaseFrame(Frame frame)
{
    switch (frame.memoryType)
    {
    case FRAME_MEMORY:
        return frameAllocator.ReleaseFrame(frame);
    case SCRATCH_MEMORY:
        return scratchAllocator.ReleaseFrame(frame);
    case STATIC_MEMORY:
        ASSERT(!"ERROR: STATIC MEMORY CANT BY FRAMED");
    }
}

