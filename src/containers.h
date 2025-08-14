#pragma once

#include <memory_manager.h>
#include <stdio.h>
#include <string.h>
#include <new>

#define INVALID_KEY ((u64)-1)

template <typename Type>
class Array
{
public:
     u32 capacity = 0;
     u32 size = 0;
     Type *data = 0;
     
     void Init(u32 size, i32 stackNum);
     void Clear();

     Type *Push(Type value);

     // TODO: this has to be const
     // CHECK ME: !!!!!!!!!!!!!!!!
     Type& operator[](u32 index) const 
     {
         return data[index];
     }

     Type& operator[](u32 index) 
     {
          return data[index];
     }

     Type& operator[](int index) const
     {
         return data[index];
     }

     Type& operator[](int index)
     {
         return data[index];
     }
};

template <typename Type>
void Array<Type>::Init(u32 capacity_, i32 stackNum)
{
    capacity = capacity_;
    size = 0;
    data = (Type*)MemoryManager::Get()->Alloc(capacity * sizeof(Type), stackNum);
    memset(data, 0, capacity * sizeof(Type));
}

template <typename Type>
void Array<Type>::Clear()
{
    size = 0;
    memset(data, 0, capacity * sizeof(Type));
}

template <typename Type>
Type* Array<Type>::Push(Type value)
{
    ASSERT(size + 1 <= capacity);
    Type* element = new (data + size) Type;
    *element = value;
    ++size;
    return element;
}

struct SlotmapKeyBase
{
     u32 id = 0;
     u64 gen = INVALID_KEY;
};

template <typename Type>
struct SlotmapKey : public SlotmapKeyBase
{
};

template <typename Type>
SlotmapKey<Type> FromKeyBase(SlotmapKeyBase keyBase)
{
     SlotmapKey<Type> key;
     key.id = keyBase.id;
     key.gen = keyBase.gen;
     return key;
}

template <typename Type>
SlotmapKeyBase FromKey(SlotmapKey<Type> key)
{
     SlotmapKeyBase keyBase;
     keyBase.id = key.id;
     keyBase.gen = key.gen;
     return key;
}

template <typename Type>
class Slotmap
{
private:
     Array<SlotmapKey<Type>> indices;
     Array<Type> data;
     Array<u32> erase;

     u64 generation = 0;
     u32 freeList = 0;
     u32 elementCount = 0;
     
public:
     void Init(u32 size, i32 stackNum);
     void Clear();

     SlotmapKey<Type> Add(Type value);
     Type *Get(SlotmapKey<Type> key);
     Array<Type> *GetArray() { return &data; };
     void Remove(SlotmapKey<Type> key);
     i32 Size();
};

template <typename Type>
void Slotmap<Type>::Init(u32 size, i32 stackNum)
{
    indices.Init(size, stackNum);
    indices.size = size;
    data.Init(size, stackNum);
    erase.Init(size, stackNum);

    freeList = 0;
    generation = 0;
    for (u32 i = 0; i < size; ++i)
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
    for (u32 i = 0; i < indices.size; ++i)
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
    SlotmapKey<Type>* freeKey = &indices.data[freeIndex];
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
Type* Slotmap<Type>::Get(SlotmapKey<Type> key)
{
    ASSERT(key.gen != INVALID_KEY);

    ASSERT(key.id < indices.capacity);
    SlotmapKey<Type> checkKey = indices.data[key.id];
    ASSERT(checkKey.gen == key.gen);
    return &data.data[checkKey.id];
}

template <typename Type>
void Slotmap<Type>::Remove(SlotmapKey<Type> key)
{
    ASSERT(key.gen != INVALID_KEY);
    ASSERT(key.id < indices.capacity);
    SlotmapKey<Type>* removeKey = &indices.data[key.id];
    if (removeKey->gen != key.gen)
    {
        printf("Key already remove!\n");
        return;
    }

    u32 indexToRemove = removeKey->id;
    removeKey->id = freeList;
    removeKey->gen = INVALID_KEY;
    freeList = key.id;

    if (indexToRemove < (data.size - 1))
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


struct FreeNode
{
     FreeNode *next = 0;
};

template <typename Type>
class ObjectAllocator
{
     static_assert(sizeof(Type) >= sizeof(FreeNode), "Object must be at least 8 bytes large");
private:
     i32 stackNum = 0;
     FreeNode *firstFree = 0;
public:
     void Init(i32 stackNum);

     Type *Alloc();
     void Free(Type *object);
};

template<typename Type>
void ObjectAllocator<Type>::Init(i32 stackNum_)
{
    firstFree = 0;
    stackNum = stackNum_;
}

template<typename Type>
Type* ObjectAllocator<Type>::Alloc()
{
    if (firstFree)
    {
        void* data = (void*)firstFree;
        firstFree = firstFree->next;
        Type* object = new (data) Type; // TODO: test this placement new
        return object;
    }
    else
    {
        void* data = MemoryManager::Get()->Alloc(sizeof(Type), stackNum);
        Type* object = new (data) Type; // TODO: test this placement new
        return object;
    }
}

template<typename Type>
void ObjectAllocator<Type>::Free(Type* object)
{
    object->~Type();
    FreeNode* free = (FreeNode*)object;
    free->next = firstFree;
    firstFree = free;
}

// Block allocator
struct BlockHeader
{
    bool occupied = false;
};

template <size_t BlockSize>
class BlockAllocator
{
    static_assert(BlockSize >= sizeof(FreeNode), "Object must be at least 8 bytes large");

public:
    struct Iterator
    {
        BlockHeader header;
        unsigned char data[BlockSize];
    };

private:
    FreeNode* firstFree = nullptr;

    unsigned char* buffer = nullptr;
    size_t capacityInBytes = 0;
    size_t bytesUsed = 0;
    size_t blockUsed = 0;
    size_t realBlockSize = 0;
    size_t blockCount = 0;
public:
    void Init(size_t blockCount, int memoryType);
    void Clear();
    void* Alloc();
    void Free(void* data);

    size_t GetBlockUsed()
    {
        return blockUsed;
    }

    size_t GetBlockCount()
    {
        return blockCount;
    }

    Iterator* GetBlockArray()
    {
        return (Iterator*)buffer;
    }
};

template<size_t BlockSize>
void BlockAllocator<BlockSize>::Init(size_t blockCount_, int memoryType)
{
    // Alloc memory for the block
    blockCount = blockCount_;
    realBlockSize = sizeof(Iterator);
    capacityInBytes = realBlockSize * blockCount;
    bytesUsed = 0;
    blockUsed = 0;
    buffer = (unsigned char *)MemoryManager::Get()->Alloc(capacityInBytes, memoryType);
    Clear();
}

template<size_t BlockSize>
void BlockAllocator<BlockSize>::Clear()
{
    // Initialize the free list
    memset((void *)buffer, 0, capacityInBytes);
    for (size_t byteOffset = 0; byteOffset < capacityInBytes; byteOffset += realBlockSize)
    {
        unsigned char* block = buffer + byteOffset;
        
        BlockHeader* header = (BlockHeader*)block;
        header->occupied = false;

        FreeNode* node = (FreeNode*)(block + sizeof(BlockHeader));

        if ((byteOffset + realBlockSize) < capacityInBytes)
        {
            node->next = (FreeNode*)(block + realBlockSize + sizeof(BlockHeader));
        }
        else
        {
            node->next = nullptr;
        }
    }
    firstFree = (FreeNode*)(buffer + sizeof(BlockHeader));
    bytesUsed = 0;
    blockUsed = 0;
}

template<size_t BlockSize>
void* BlockAllocator<BlockSize>::Alloc()
{
    ASSERT(firstFree);

    BlockHeader* header = (BlockHeader*)((unsigned char*)firstFree - sizeof(BlockHeader));
    ASSERT(header->occupied == false);
    header->occupied = true;

    void* memory = firstFree;
    firstFree = firstFree->next;

    blockUsed++;
    bytesUsed += realBlockSize;

    return memory;
}

template<size_t BlockSize>
void BlockAllocator<BlockSize>::Free(void* object)
{
    BlockHeader* header = (BlockHeader*)((unsigned char*)object - sizeof(BlockHeader));
    ASSERT(header->occupied == true);
    header->occupied = false;

    FreeNode* free = (FreeNode*)object;
    free->next = firstFree;
    firstFree = free;

    blockUsed--;
    bytesUsed -= realBlockSize;
}

#define INVALID_MAP_ID 0xFFFFFFFF
#define DELETED_MAP_ID (0xFFFFFFFF - 1)

template<typename Type>
class HashMap
{
public:
     struct HashElement
     {
          u32 id;
          Type value;
     };
     
     void Init(u64 size, i32 stackNum);
     
     void Add(const char *name, Type value);
     void Add(i32 key, Type value);
     void Remove(const char *name);
     void Remove(i32 key);
     Type *Get(const char *name);
     Type *Get(i32 key);
     bool Contains(const char *name);
     int Count()
     {
         return occupied;
     }
     void PrintHashMap();
     
private:
     HashElement *elements = 0;
     u32 mask = 0;
     u32 capacity = 0;
     u32 occupied = 0;
};

u32 MurMur2(const void* key, i32 len, u32 seed);

template<typename Type>
void HashMap<Type>::Init(u64 capacity_, i32 stackNum)
{
    ASSERT(capacity_ > 1);
    // TODO: remove this
    ASSERT((capacity_ & (capacity_ - 1)) == 0); // capacity_ should be power of two
    capacity = capacity_;
    mask = capacity - 1;

    u64 size = capacity * sizeof(HashElement);
    elements = (HashElement*)MemoryManager::Get()->Alloc(size, stackNum);
    memset(elements, 0, size);

    occupied = 0;

    for (i32 i = 0; i < capacity; ++i)
    {
        elements[i].id = INVALID_MAP_ID;
    }
}

// TODO: check if we need to use placement new
template<typename Type>
void HashMap<Type>::Add(const char* name, Type value)
{
    ASSERT(occupied + 1 <= capacity);

    u32 id = MurMur2(name, strlen(name), 123);
    u32 index = id % mask;

    if (elements[index].id == INVALID_MAP_ID ||
        elements[index].id == DELETED_MAP_ID)
    {
        elements[index].id = id;
        elements[index].value = value;
        occupied++;
    }
    else
    {
        if (elements[index].id == id)
        {
            ASSERT(!"Element is already on the map!");
        }

        u32 nextIndex = (index + 1) % capacity;
        while (elements[nextIndex].id != INVALID_MAP_ID &&
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

    if (elements[index].id == INVALID_MAP_ID ||
        elements[index].id == DELETED_MAP_ID)
    {
        elements[index].id = id;
        elements[index].value = value;
        occupied++;
    }
    else
    {
        if (elements[index].id == id)
        {
            ASSERT(!"Element is already on the map!");
        }

        u32 nextIndex = (index + 1) % capacity;
        while (elements[nextIndex].id != INVALID_MAP_ID &&
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
void HashMap<Type>::Remove(const char* name)
{
    u32 id = MurMur2(name, strlen(name), 123);
    u32 index = id % mask;

    if (elements[index].id == INVALID_MAP_ID)
    {
        ASSERT(!"Error: trying to delete an invalid element!");
    }

    i32 checkCount = 0;
    while (elements[index].id != id)
    {
        index = (index + 1) % capacity;
        if (checkCount >= capacity)
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

    if (elements[index].id == INVALID_MAP_ID)
    {
        ASSERT(!"Error: trying to delete an invalid element!");
    }

    i32 checkCount = 0;
    while (elements[index].id != id)
    {
        index = (index + 1) % capacity;
        if (checkCount >= capacity)
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
Type* HashMap<Type>::Get(const char* name)
{

    u32 id = MurMur2(name, strlen(name), 123);
    u32 index = id % mask;

    if (elements[index].id == INVALID_MAP_ID)
    {
        ASSERT(!"Error: trying to get an invalid element!");
    }

    i32 checkCount = 0;
    while (elements[index].id != id &&
        elements[index].id != INVALID_MAP_ID)
    {
        index = (index + 1) % capacity;
        if (checkCount >= capacity)
        {
            ASSERT(!"Error: element not found!");
        }
        checkCount++;
    }

    if (elements[index].id == INVALID_MAP_ID)
    {
        ASSERT(!"Error: element not found!");
    }

    return &elements[index].value;
}

template<typename Type>
Type* HashMap<Type>::Get(i32 key)
{

    u32 id = MurMur2(&key, sizeof(key), 123);
    u32 index = id % mask;

    if (elements[index].id == INVALID_MAP_ID)
    {
        ASSERT(!"Error: trying to get an invalid element!");
    }

    i32 checkCount = 0;
    while (elements[index].id != id &&
        elements[index].id != INVALID_MAP_ID)
    {
        index = (index + 1) % capacity;
        if (checkCount >= capacity)
        {
            ASSERT(!"Error: element not found!");
        }
        checkCount++;
    }

    if (elements[index].id == INVALID_MAP_ID)
    {
        ASSERT(!"Error: element not found!");
    }

    return &elements[index].value;
}

template<typename Type>
bool HashMap<Type>::Contains(const char* name)
{
    u32 id = MurMur2(name, strlen(name), 123);
    u32 index = id % mask;

    if (elements[index].id == INVALID_MAP_ID)
    {
        return false;
    }

    i32 checkCount = 0;
    while (elements[index].id != id &&
        elements[index].id != INVALID_MAP_ID)
    {
        index = (index + 1) % capacity;
        if (checkCount >= capacity)
        {
            return false;
        }
        checkCount++;
    }

    if (elements[index].id == INVALID_MAP_ID)
    {
        return false;
    }

    return true;
}

template<typename Type>
void HashMap<Type>::PrintHashMap()
{
    for (i32 i = 0; i < capacity; ++i)
    {
        switch (elements[i].id)
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