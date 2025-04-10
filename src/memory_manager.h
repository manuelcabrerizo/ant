#pragma once

class MemoryManager
{
private:
     MemoryManager() {}
     static MemoryManager instance;
     static bool initialize;

     DoubleStackAllocator allocator;
public:
     static void Init(u64 memorySize, size_t align);
     static void Terminate();
     static MemoryManager *Get();

     void *Alloc(u64 size, i32 stackNum);
     Frame GetFrame();

     void ReleaseFrame(Frame frame);
};
