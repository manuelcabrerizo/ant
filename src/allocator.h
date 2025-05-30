#pragma once

#include "common.h"

#define ALIGNUP(addr, align) ((((int)addr) + (align)-1) & (~((align)-1)))

uintptr_t AlignAddress(uintptr_t addr, size_t align);

template<typename Type>
Type *AlignPointer(Type *ptr, size_t align)
{
     uintptr_t addr = (uintptr_t)ptr;
     uintptr_t addrAlign = AlignAddress(addr, align);
     return (Type *)addrAlign;
}

struct Frame
{
     u8 *frame;
     i32 stackNum;
};

#define FRAME_MEMORY 0
#define STATIC_MEMORY 1

class DoubleStackAllocator
{
private:
     i32 byteAlignment;
     u8 *memoryBlock;
     u8 *baseAndCap[2];
     u8 *head[2];
public:     
     void Init(u64 size, size_t byteAligment);
     void Terminate();

     void *Alloc(u64 size, i32 stackNum);
     Frame GetFrame(i32 stackNum);
     void ReleaseFrame(Frame frame);

     size_t GetFreeMemoryCount()
     {
         return head[1] - head[0];
     }
};
