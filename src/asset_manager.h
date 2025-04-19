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

     virtual void Unload(const char *name) = 0;
protected:
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
     void Load(const char *name, const char *path);
     void Unload(const char *name) override;
     Texture *Get(const char *name);
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
     void Load(const char *name, const char *path);
     void Unload(const char *name) override;
     Model *Get(const char *name);
};

struct ShaderHandle
{
     const char *name;
     Shader *shader;
};

class ShaderManager : public AssetManager<ShaderHandle>
{
public:
     void Load(const char *name, const char *vertPath, const char *fragPath);
     void Unload(const char *name) override;
     void Bind(const char *name);
     Shader *Get(const char *name);
};