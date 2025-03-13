#pragma once

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

     Type& operator[](u32 index) {
          return data[index];
     }
     
};

template <typename Type>
struct SlotmapKey
{
     u32 id = 0;
     u64 gen = INVALID_KEY;
};

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
     void Remove(const char *name);
     Type *Get(const char *name);

     void PrintHashMap();
     
private:
     HashElement *elements = 0;
     u32 mask = 0;
     u32 capacity = 0;
     u32 occupied = 0;
};
