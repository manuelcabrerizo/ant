#include "allocator.h"
#include <cstdlib>
#include <memory>

/*
uintptr_t AlignAddress(uintptr_t addr, size_t align)
{
    size_t mask = align - 1;
    ASSERT((align & mask) == 0); // align should be power of 2
    return  (addr + mask) & ~mask;
}
*/

// StackAllocator
void StackAllocator::Init(u64 size)
{
    // First allocate our memory block
    memoryBlock = (u8*)malloc(size);
    if (!memoryBlock)
    {
        ASSERT(!"Error allocating memory");
    }
    memset(memoryBlock, 0, size);

    // Set up base pointer
    base = memoryBlock;
    // Set up cap pointer
    cap = memoryBlock + size;
    // Finaly initialize the lower and upper frame pointers
    head = base;
}

void StackAllocator::Terminate()
{
    free(memoryBlock);
}

void* StackAllocator::Alloc(u64 size)
{
    u8* mem = 0;

    // check for available memory
    if (head + size > cap)
    {
        // insufficient memory
        ASSERT(!"insufficient memory");
        return 0;
    }

    mem = head;
    head += size;

    return mem;
}

Frame StackAllocator::GetFrame(int memoryType)
{
    Frame frame;
    frame.memoryType = memoryType;
    frame.frame = head;
    return frame;
}

void StackAllocator::ReleaseFrame(Frame frame)
{
    head = frame.frame;
}