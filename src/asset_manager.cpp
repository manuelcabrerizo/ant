template <typename Type>
void AssetManager<Type>::Init(u32 assetsCapacity)
{
     nameIndex.Init(assetsCapacity * sizeof(Type), STATIC_MEMORY);
     assets.Init(assetsCapacity, STATIC_MEMORY);
}

template <typename Type>
void AssetManager<Type>::Terminate()
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

// Model Manager
void ModelManager::Load(const char *name, const char *path)
{
     ModelHandle modelHandle;
     modelHandle.name = name;
     modelHandle.model.Init(path);
     nameIndex.Add(name, assets.Add(modelHandle));
}

void ModelManager::Unload(const char *name)
{
     auto handle = *nameIndex.Get(name);
     assets.Get(handle)->model.Terminate();
     assets.Remove(handle);
     nameIndex.Remove(name);
}