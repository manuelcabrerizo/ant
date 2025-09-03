#pragma once

#include "allocator.h"

#define FRAME_MEMORY 0
#define SCRATCH_MEMORY 1
#define STATIC_MEMORY 2

class MemoryManager
{
private:
     MemoryManager() {}
     static MemoryManager instance;
     static bool initialize;
    
    StackAllocator staticAllocator{};
    StackAllocator frameAllocator{};
    StackAllocator scratchAllocator{};

public:
     static void Init(u64 staticMemorySize, u64 frameMemorySize, u64 scratchMemorySize);
     static void Terminate();
     static MemoryManager *Get();

     void *Alloc(u64 size, i32 memoryType);
     
     Frame GetFrame(i32 memoryType);
     void ReleaseFrame(Frame frame);

     size_t GetFreeMemoryCount(i32 memoryType)
     {
         switch (memoryType)
         {
         case FRAME_MEMORY:
             return frameAllocator.GetFreeMemoryCount();
         case SCRATCH_MEMORY:
             return scratchAllocator.GetFreeMemoryCount();
         case STATIC_MEMORY:
             return staticAllocator.GetFreeMemoryCount();
         }
     }
};