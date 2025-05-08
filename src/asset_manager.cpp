template <typename Type>
void AssetManager<Type>::Init(u32 assetsCapacity)
{
     nameIndex.Init(assetsCapacity, STATIC_MEMORY);
     assets.Init(assetsCapacity, STATIC_MEMORY);
}

template <typename Type>
void AssetManager<Type>::Terminate()
{
     Clear();
}

template <typename Type>
void AssetManager<Type>::Clear()
{
     for(i32 i = assets.GetArray()->size - 1; i >= 0; --i)
     {
          Type *asset = &assets.GetArray()->data[i];
          Unload(asset->name);
     }
}
     
template <typename Type>
Type *AssetManager<Type>::Get(const char *name)
{
     return assets.Get(*nameIndex.Get(name));
}

template <typename Type>
Type *AssetManager<Type>::Get(SlotmapKey<Type> handle)
{
     return assets.Get(handle);
}

template <typename Type>
SlotmapKey<Type> AssetManager<Type>::GetHandle(const char *name)
{
     return *nameIndex.Get(name);
}


// Texture Manager
void TextureManager::Load(const char *name, const char *path)
{
     TextureHandle textureHandle;
     textureHandle.name = name;
     textureHandle.texture = GraphicsManager::Get()->TextureAlloc(path);
     nameIndex.Add(name, assets.Add(textureHandle));
}

void TextureManager::Unload(const char *name)
{
     auto handle = *nameIndex.Get(name);
     GraphicsManager::Get()->TextureFree(assets.Get(handle)->texture);
     assets.Remove(handle);
     nameIndex.Remove(name);
}

Texture *TextureManager::Get(const char *name)
{
     return AssetManager::Get(name)->texture;
}


// Model Manager
void ModelManager::Load(const char *name, const char *path)
{
     ModelHandle modelHandle;
     modelHandle.name = name;
     modelHandle.model.Init(path, STATIC_MEMORY);
     nameIndex.Add(name, assets.Add(modelHandle));
}

void ModelManager::Unload(const char *name)
{
     auto handle = *nameIndex.Get(name);
     assets.Get(handle)->model.Terminate();
     assets.Remove(handle);
     nameIndex.Remove(name);
}

Model *ModelManager::Get(const char *name)
{
     return &AssetManager::Get(name)->model;
}


void ShaderManager::Load(const char *name, const char *vertPath, const char *fragPath)
{
     Frame frame = MemoryManager::Get()->GetFrame();
     File vertFile = PlatformReadFile(vertPath, FRAME_MEMORY);
     File fragFile = PlatformReadFile(fragPath, FRAME_MEMORY);
     ShaderHandle shaderHandle;
     shaderHandle.name = name;
     shaderHandle.shader =  GraphicsManager::Get()->ShaderAlloc(vertFile, fragFile);
     MemoryManager::Get()->ReleaseFrame(frame);
     nameIndex.Add(name, assets.Add(shaderHandle));
}

void ShaderManager::Unload(const char *name)
{
     auto handle = *nameIndex.Get(name);
     GraphicsManager::Get()->ShaderFree(assets.Get(handle)->shader);
     assets.Remove(handle);
     nameIndex.Remove(name);
}

void ShaderManager::Bind(const char *name)
{
     GraphicsManager::Get()->ShaderBind(AssetManager::Get(name)->shader);
}

Shader *ShaderManager::Get(const char *name)
{
     return AssetManager::Get(name)->shader;
}

