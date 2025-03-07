#pragma once

#define INVALID_KEY ((u64)-1)

template <typename Type>
class Array
{
public:
     u32 capacity = 0;
     u32 size = 0;
     Type *data = 0;
     
     void Init(u32 size, Arena *arena);
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

     u32 freeList = 0;
     u64 generation = 0;
public:
     void Init(u32 size, Arena *arena);
     void Clear();

     SlotmapKey<Type> Add(Type value);
     Type *Get(SlotmapKey<Type> key);
     Array<Type> *GetArray() { return &data; };
     void Remove(SlotmapKey<Type> key);
};
