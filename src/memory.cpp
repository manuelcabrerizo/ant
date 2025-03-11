#include <memory.h>

void DoubleStackAllocator::Init(u64 size, size_t byteAlignment_)
{
     // first make sure size if a multiple of byteAligment
     size = ALIGNUP(size, byteAlignment_);

     // First allocate our memory block
     memoryBlock = (u8 *)malloc(size + byteAlignment_);
     if(!memoryBlock)
     {
          ASSERT(!"Error allocating memory");
     }
     
     byteAlignment = byteAlignment_;

     // Set up base pointer
     baseAndCap[0] = AlignPointer<u8>(memoryBlock, byteAlignment);
     // Set up cap pointer
     baseAndCap[1] = AlignPointer<u8>(memoryBlock + size, byteAlignment);

     // Finaly initialize the lower and upper frame pointers
     head[0] = baseAndCap[0];
     head[1] = baseAndCap[1];
}

void DoubleStackAllocator::Terminate()
{
     free(memoryBlock);
}

void *DoubleStackAllocator::Alloc(u64 size, i32 stackNum)
{
     u8 *mem = 0;

     // first align the requested size
     size = ALIGNUP(size, byteAlignment);

     // check for available memory
     if(head[0] + size > head[1])
     {
          // insufficient memory
          ASSERT(!"insufficient memory");
          return 0;
     }

     // now perform the memory allocation
     if(stackNum)
     {
          // Allocating from the upper stack, down
          head[1] -= size;
          mem = head[1];
     }
     else
     {
          // Allocating from the lower stack, up
          mem = head[0];
          head[0] += size;
     }
     
     return (void *)mem;
}

Frame DoubleStackAllocator::GetFrame(i32 stackNum)
{
     Frame frame;
     frame.frame = head[stackNum];
     frame.stackNum = stackNum;
     return frame;
}

void DoubleStackAllocator::ReleaseFrame(Frame frame)
{
     head[frame.stackNum] = frame.frame;
}
