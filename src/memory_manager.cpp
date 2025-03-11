MemoryManager MemoryManager::instance;
bool MemoryManager::initialize = false;

void MemoryManager::Init(u64 memorySize, size_t align)
{
     if(initialize)
     {
          ASSERT(!"Error: memory manager already initialize");
     }
     memset(&instance, 0, sizeof(MemoryManager));
     instance.allocator.Init(memorySize, align);
     initialize = true;
}

void MemoryManager::Terminate()
{
     if(!initialize)
     {
          ASSERT(!"Error: memory manager has not been initialize");
     }
     instance.allocator.Terminate();
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

void *MemoryManager::Alloc(u64 size, i32 stackNum)
{
     return allocator.Alloc(size, stackNum);
}

Frame MemoryManager::GetFrame(i32 stackNum)
{
     return allocator.GetFrame(stackNum);
}

void MemoryManager::ReleaseFrame(Frame frame)
{
     allocator.ReleaseFrame(frame);
}

