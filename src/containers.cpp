template <typename Type>
void Array<Type>::Init(u32 capacity_, i32 stackNum)
{
     capacity = capacity_;
     size = 0;
     data = (Type *)MemoryManager::Get()->Alloc(capacity * sizeof(Type), stackNum);
     memset(data, 0, capacity * sizeof(Type));
}

template <typename Type>
void Array<Type>::Clear()
{
     size = 0;
     memset(data, 0, capacity * sizeof(Type));
}

template <typename Type>
Type *Array<Type>::Push(Type value)
{
     ASSERT(size + 1 <= capacity);
     Type *element = new (data + size) Type;
     *element = value;
     ++size;
     return element;
}

template <typename Type>
void Slotmap<Type>::Init(u32 size, i32 stackNum)
{
     indices.Init(size, stackNum);
     indices.size = size;
     data.Init(size, stackNum);
     erase.Init(size, stackNum);

     freeList = 0;
     generation = 0;
     for(u32 i = 0; i < size; ++i)
     {
          indices.data[i].id = i + 1;
          indices.data[i].gen = INVALID_KEY;
     }
}

template <typename Type>
void Slotmap<Type>::Clear()
{
     data.size = 0;
     erase.size = 0;
     freeList = 0;
     generation = 0;
     elementCount = 0;
     for(u32 i = 0; i < indices.size; ++i)
     {
          indices.data[i].id = i + 1;
          indices.data[i].gen = INVALID_KEY;
     }
}

template <typename Type>
SlotmapKey<Type> Slotmap<Type>::Add(Type value)
{
     ASSERT(freeList < indices.capacity);

     u32 freeIndex = freeList;
     SlotmapKey<Type> *freeKey = &indices.data[freeIndex];
     freeList = freeKey->id;

     ASSERT(generation != INVALID_KEY);
     freeKey->id = data.size;
     freeKey->gen = generation++;

     data.Push(value);
     erase.Push(freeIndex);

     elementCount++;

     SlotmapKey<Type> key;
     key.id = freeIndex;
     key.gen = freeKey->gen;
     return key;
}

template <typename Type>
Type *Slotmap<Type>::Get(SlotmapKey<Type> key)
{
     ASSERT(key.gen != INVALID_KEY);

     ASSERT(key.id < indices.capacity);
     SlotmapKey<Type> checkKey =  indices.data[key.id];
     ASSERT(checkKey.gen == key.gen);
     return &data.data[checkKey.id];
}

template <typename Type>
void Slotmap<Type>::Remove(SlotmapKey<Type> key)
{
     ASSERT(key.id < indices.capacity);
     SlotmapKey<Type> *removeKey =  &indices.data[key.id];
     if(removeKey->gen != key.gen)
     {
          printf("Key already remove!\n");
          return;
     }

     u32 indexToRemove = removeKey->id;
     removeKey->id = freeList;
     removeKey->gen = INVALID_KEY;
     freeList = key.id;

     if(indexToRemove < (data.size - 1))
     {
          data.data[indexToRemove] = data.data[data.size - 1];
          erase.data[indexToRemove] = erase.data[erase.size - 1];
          indices.data[erase.data[indexToRemove]].id = indexToRemove;
     }
     --data.size;
     --erase.size;
     --elementCount;
}

template<typename Type>
i32 Slotmap<Type>::Size()
{
     ASSERT(data.size == elementCount);
     return elementCount;
}

template<typename Type>
void ObjectAllocator<Type>::Init(i32 stackNum_)
{
     firstFree = 0;
     stackNum = stackNum_;
}

template<typename Type>
Type *ObjectAllocator<Type>::Alloc()
{
     if(firstFree)
     {
          void *data = (void *)firstFree;
          firstFree = firstFree->next;
          Type *object = new (data) Type; // TODO: test this placement new
          return object;
     }
     else
     {
          void *data = MemoryManager::Get()->Alloc(sizeof(Type), stackNum);
          Type *object = new (data) Type; // TODO: test this placement new
          return object;
     }
}

template<typename Type>
void ObjectAllocator<Type>::Free(Type *object)
{
     FreeNode *free = (FreeNode *)object;
     free->next = firstFree;
     firstFree = free;
}

// TODO: mabye use 64 bits hash
/*
static u32 djb2(const char *str) {
    u32 hash = 5381;
    int c;
    while ((c = *str++))
    {
        hash = ((hash << 5) + hash) + c;
    }
    return hash;
}
*/
static u32 MurMur2(const void *key, i32 len, u32 seed) {
     const u32 m = 0x5bd1e995;
     const i32 r = 24;
     u32 h = seed ^ len;
     const u8 *data = (const u8 *)key;
     while(len >= 4) {
         u32 k = *(u32 *)data;
         k *= m;
         k ^= k >> r;
         k *= m;
         h *= m;
         h ^= k;
         data += 4;
         len -= 4;
     }
     switch(len) {
         case 3: h ^= data[2] << 16;
         case 2: h ^= data[1] << 8;
         case 1: h ^= data[0];
                 h *= m;
     }
     h ^= h >> 13;
     h *= m;
     h ^= h >> 15;
     return h;
 }


template<typename Type>
void HashMap<Type>::Init(u64 capacity_, i32 stackNum)
{
     ASSERT(capacity_ > 1);
     ASSERT((capacity_ & (capacity_ - 1)) == 0); // capacity_ should be power of two
     capacity = capacity_;
     mask = capacity - 1;

     u64 size = capacity * sizeof(HashElement);
     elements = (HashElement *)MemoryManager::Get()->Alloc(size, stackNum);
     memset(elements, 0, size);
     
     occupied = 0;

     for(i32 i = 0; i < capacity; ++i)
     {
          elements[i].id = INVALID_MAP_ID;
     }
}

// TODO: Test this to make shoure if we need to use placement new
template<typename Type>
void HashMap<Type>::Add(const char *name, Type value)
{
     ASSERT(occupied + 1 <= capacity);
     
     u32 id = MurMur2(name, strlen(name), 123);
     u32 index = id % mask;

     if(elements[index].id == INVALID_MAP_ID ||
        elements[index].id == DELETED_MAP_ID)
     {
          elements[index].id = id;
          elements[index].value = value;
          occupied++;
     }
     else
     {
          if(elements[index].id == id)
          {
               ASSERT(!"Element is already on the map!");
          }

          u32 nextIndex = (index + 1) % capacity;
          while(elements[nextIndex].id != INVALID_MAP_ID &&
                elements[nextIndex].id != DELETED_MAP_ID)
          {
               nextIndex = (nextIndex + 1) % capacity;
          }
          elements[nextIndex].id = id;
          elements[nextIndex].value = value;
          occupied++;
     }
}

template<typename Type>
void HashMap<Type>::Add(i32 key, Type value)
{
     ASSERT(occupied + 1 <= capacity);
     
     u32 id = MurMur2(&key, sizeof(key), 123);
     u32 index = id % mask;

     if(elements[index].id == INVALID_MAP_ID ||
        elements[index].id == DELETED_MAP_ID)
     {
          elements[index].id = id;
          elements[index].value = value;
          occupied++;
     }
     else
     {
          if(elements[index].id == id)
          {
               ASSERT(!"Element is already on the map!");
          }

          u32 nextIndex = (index + 1) % capacity;
          while(elements[nextIndex].id != INVALID_MAP_ID &&
                elements[nextIndex].id != DELETED_MAP_ID)
          {
               nextIndex = (nextIndex + 1) % capacity;
          }
          elements[nextIndex].id = id;
          elements[nextIndex].value = value;
          occupied++;
     }
}

template<typename Type>
void HashMap<Type>::Remove(const char *name)
{
     u32 id = MurMur2(name, strlen(name), 123);
     u32 index = id % mask;

     if(elements[index].id == INVALID_MAP_ID)
     {
          ASSERT(!"Error: trying to delete an invalid element!");
     }

     i32 checkCount = 0;
     while(elements[index].id != id)
     {
          index = (index + 1) % capacity;
          if(checkCount >= capacity)
          {
               ASSERT(!"Error: element not found!");
          }
          checkCount++;
     }
     elements[index].id = DELETED_MAP_ID;
     elements[index].value = {};
     occupied--;
}

template<typename Type>
void HashMap<Type>::Remove(i32 key)
{
     u32 id = MurMur2(&key, sizeof(key), 123);
     u32 index = id % mask;

     if(elements[index].id == INVALID_MAP_ID)
     {
          ASSERT(!"Error: trying to delete an invalid element!");
     }

     i32 checkCount = 0;
     while(elements[index].id != id)
     {
          index = (index + 1) % capacity;
          if(checkCount >= capacity)
          {
               ASSERT(!"Error: element not found!");
          }
          checkCount++;
     }
     elements[index].id = DELETED_MAP_ID;
     elements[index].value = {};
     occupied--;
}

template<typename Type>
Type *HashMap<Type>::Get(const char *name)
{

     u32 id = MurMur2(name, strlen(name), 123);
     u32 index = id % mask;

     if(elements[index].id == INVALID_MAP_ID)
     {
          ASSERT(!"Error: trying to get an invalid element!");
     }

     i32 checkCount = 0;
     while(elements[index].id != id &&
           elements[index].id != INVALID_MAP_ID)
     {
          index = (index + 1) % capacity;
          if(checkCount >= capacity)
          {
               ASSERT(!"Error: element not found!");
          }
          checkCount++;
     }
     
     if(elements[index].id == INVALID_MAP_ID)
     {
          ASSERT(!"Error: element not found!");
     }
     
     return &elements[index].value;
}

template<typename Type>
Type *HashMap<Type>::Get(i32 key)
{

     u32 id = MurMur2(&key, sizeof(key), 123);
     u32 index = id % mask;

     if(elements[index].id == INVALID_MAP_ID)
     {
          ASSERT(!"Error: trying to get an invalid element!");
     }

     i32 checkCount = 0;
     while(elements[index].id != id &&
           elements[index].id != INVALID_MAP_ID)
     {
          index = (index + 1) % capacity;
          if(checkCount >= capacity)
          {
               ASSERT(!"Error: element not found!");
          }
          checkCount++;
     }
     
     if(elements[index].id == INVALID_MAP_ID)
     {
          ASSERT(!"Error: element not found!");
     }
     
     return &elements[index].value;
}

template<typename Type>
void HashMap<Type>::PrintHashMap()
{
     for(i32 i = 0; i < capacity; ++i)
     {
          switch(elements[i].id)
          {
          case INVALID_MAP_ID:
          {
               printf("INVALID_MAP_ID\n");
          } break;
          case DELETED_MAP_ID:
          {
               printf("DELETED_MAP_ID\n");
          } break;
          default:
          {
               printf("OCCUPIED\n");
          } break;
          }
     }
}
