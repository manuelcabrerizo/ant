#pragma once

template <typename Type>
class AssetManager
{
protected:
     Slotmap<Type> assets;
     HashMap<SlotmapKey<Type>> nameIndex;
public:
     virtual ~AssetManager() {}
     
     void Init(u32 assetsCapacity);
     void Terminate();

     virtual void Load(const char *name, const char *path) = 0;
     virtual void Unload(const char *name) = 0;
     
     Type *Get(const char *name);
     Type *Get(SlotmapKey<Type> handle);
     SlotmapKey<Type> GetHandle(const char *name);
};

// Texture Manager
struct TextureHandle
{
     const  char *name;
     Texture *texture;
};

class TextureManager : public AssetManager<TextureHandle>
{
public:
     void Load(const char *name, const char *path) override;
     void Unload(const char *name) override;
};

// Model Manager
struct ModelHandle
{
     const char *name;
     Model model;
};


class ModelManager : public AssetManager<ModelHandle>
{
public:
     void Load(const char *name, const char *path) override;
     void Unload(const char *name) override;
};
