#pragma once

#define ALIGNUP(addr, align) ((((int)addr) + (align)-1) & (~((align)-1)))

uintptr_t AlignAddress(uintptr_t addr, size_t align)
{
     size_t mask = align - 1;
     ASSERT((align & mask) == 0); // align should be power of 2
     return  (addr + mask) & ~mask;
}

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

#define STACK_LOW 0
#define STACK_UP 1

class DobleStackAllocator
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
     
};
