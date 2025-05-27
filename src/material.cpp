#include "material.h"
#include <asset_manager.h>

#include <string.h>

#include <windows.h>


void Material::Init(const char* shaderName, ShaderManager *shaderManager)
{
    size_t shaderNameLen = strlen(shaderName);
    ASSERT(shaderNameLen <= _MAX_PATH);
    memset(this->shaderName, 0, _MAX_PATH);
    memcpy(this->shaderName, shaderName, shaderNameLen);

    ASSERT(shaderManager->Contains(shaderName));
    this->shader = shaderManager->Get(shaderName);
}

void Material::Terminate()
{
    this->shader = nullptr;
    OutputDebugString("Material Terminate()\n");
}

void SolidColorMaterial::Init(const char* shaderName,
                              ShaderManager* shaderManager,
                              const Vector3& ambient,
                              const Vector3& diffuse,
                              const Vector3& specular,
                              f32 shininess)
{
    Material::Init(shaderName, shaderManager);
    this->ambient = ambient;
    this->diffuse = diffuse;
    this->specular = specular;
    this->shininess = shininess;
}

void SolidColorMaterial::Terminate()
{
    Material::Terminate();
    OutputDebugString("SolidColorMaterial Terminate()\n");
}

void SolidColorMaterial::Bind()
{
    OutputDebugString("SolidColorMaterial Bind()\n");
}

void TextureMaterial::Init(const char* shaderName,
                           ShaderManager* shaderManager,
                           const char* diffuseName,
                           const char* normalName,
                           const char* specularName,
                           f32 shininess, 
                           TextureManager *textureManager)
{
    Material::Init(shaderName, shaderManager);

    size_t diffuseNameLen = strlen(diffuseName);
    ASSERT(diffuseNameLen <= _MAX_PATH);
    memset(this->diffuseName, 0, _MAX_PATH);
    memcpy(this->diffuseName, diffuseName, diffuseNameLen);

    size_t normalNameLen = strlen(normalName);
    ASSERT(normalNameLen <= _MAX_PATH);
    memset(this->normalName, 0, _MAX_PATH);
    memcpy(this->normalName, normalName, normalNameLen);

    size_t specularNameLen = strlen(specularName);
    ASSERT(specularNameLen <= _MAX_PATH);
    memset(this->specularName, 0, _MAX_PATH);
    memcpy(this->specularName, specularName, specularNameLen);

    ASSERT(textureManager->Contains(diffuseName));
    this->diffuse = textureManager->Get(diffuseName);

    ASSERT(textureManager->Contains(normalName));
    this->normal = textureManager->Get(normalName);

    ASSERT(textureManager->Contains(specularName));
    this->specular = textureManager->Get(specularName);

    this->shininess = shininess;
}

void TextureMaterial::Terminate()
{
    Material::Terminate();
    this->diffuse = nullptr;
    this->normal = nullptr;
    this->specular = nullptr;   
    OutputDebugString("TextureMaterial Terminate()\n");
}

void TextureMaterial::Bind()
{
    OutputDebugString("TextureMaterial Bind()\n");
}