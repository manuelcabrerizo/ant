#include <memory.h>

u32 Arena::allocationCount;

void Arena::Init(u64 size_)
{
     base = malloc(size_);
     allocated = true;
     size = size_;
     used = 0;
     allocationCount++;
}

void Arena::Init(Arena *memory, u64 size_)
{
     base = memory->PushSize(size_);
     allocated = false;
     size = size_;
     used = 0;
}

void Arena::Terminate()
{
     if(allocated)
     {
          free(base);
          allocationCount--;
     }
     base = 0;
     size = 0;
     used = 0;
     allocated = false;
}

void Arena::Clear()
{
     used = 0;
}

void *Arena::PushSize(u64 size_)
{
     ASSERT(used + size_ <= size);
     void *data = (u8 *)base + used;
     used += size_;
     return data;
}

void Arena::PopSize(u64 size_)
{
     ASSERT(used - size_ >= 0);
     used -= size_;
}
