#pragma once

#include "common.h"

/*
#define ALIGNUP(addr, align) ((((int)addr) + (align)-1) & (~((align)-1)))

uintptr_t AlignAddress(uintptr_t addr, size_t align);

template<typename Type>
Type *AlignPointer(Type *ptr, size_t align)
{
     uintptr_t addr = (uintptr_t)ptr;
     uintptr_t addrAlign = AlignAddress(addr, align);
     return (Type *)addrAlign;
}
*/

class StackAllocator;
class DoubleStackAllocator;
class MemoryManager;

class Frame
{
    friend StackAllocator;
    friend DoubleStackAllocator;
    friend MemoryManager;

    u8* frame;
    i32 memoryType;
};

class StackAllocator
{
private:
    u8* memoryBlock;
    u8 *base;
    u8* cap;
    u8* head;
public:
    void Init(size_t size);
    void Terminate();

    void* Alloc(u64 size);
    Frame GetFrame(int memoryType);
    void ReleaseFrame(Frame frame);

    size_t GetFreeMemoryCount()
    {
        return cap - head;
    }

};
