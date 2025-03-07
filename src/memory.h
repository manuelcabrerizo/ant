#pragma once

class Arena
{
private:
     void *base;
     u64 size;
     u64 used;
     bool allocated;

public:
     static u32 allocationCount;
     
     void Init(u64 size);
     void Init(Arena *memory, u64 size);
     void Terminate();

     void *PushSize(u64 size);
     void PopSize(u64 size);
     void Clear();
};
