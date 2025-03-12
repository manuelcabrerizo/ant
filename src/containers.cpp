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
