#include "material.h"

#include <asset_managers/shader_manager.h>
#include <asset_managers/texture_manager.h>

#include <string.h>


void Material::Init(const char* shaderName)
{
    size_t shaderNameLen = strlen(shaderName);
    ASSERT(shaderNameLen <= _MAX_PATH);
    memset(this->shaderName, 0, _MAX_PATH);
    memcpy(this->shaderName, shaderName, shaderNameLen);

    ASSERT(FragmentShaderManager::Get()->Contains(shaderName));
    this->shader = FragmentShaderManager::Get()->Get(shaderName);
}

void Material::Terminate()
{
    this->shader = nullptr;
}

i32 SolidColorMaterial::instanceCount = 0;
UniformBuffer* SolidColorMaterial::uniformBuffer = nullptr;

void SolidColorMaterial::Init(
    const char* shaderName,
    const Vector3& ambient,
    const Vector3& diffuse,
    const Vector3& specular,
    f32 shininess)
{
    Material::Init(shaderName);
    this->ambient = ambient;
    this->diffuse = diffuse;
    this->specular = specular;
    this->shininess = shininess;

    ubo.ambient = this->ambient;
    ubo.diffuse = this->diffuse;
    ubo.specular = this->specular;
    ubo.shininess = this->shininess;
    if (instanceCount == 0)
    {
        uniformBuffer = GraphicsManager::Get()->UniformBufferAlloc(BIND_TO_PS, &ubo, sizeof(ubo), 0);
    }
    instanceCount++;
}

void SolidColorMaterial::Terminate()
{
    Material::Terminate();
    instanceCount--;
    ASSERT(instanceCount >= 0);
    if (instanceCount == 0)
    {
        GraphicsManager::Get()->UniformBufferFree(uniformBuffer);
    }
}

void SolidColorMaterial::Bind()
{
    GraphicsManager::Get()->UniformBufferBind(uniformBuffer);
    ubo.ambient = this->ambient;
    ubo.diffuse = this->diffuse;
    ubo.specular = this->specular;
    ubo.shininess = this->shininess;
    GraphicsManager::Get()->UniformBufferUpdate(uniformBuffer, &ubo);


    GraphicsManager::Get()->FragmentShaderBind(shader);
}

i32 TextureMaterial::instanceCount = 0;
UniformBuffer* TextureMaterial::uniformBuffer = nullptr;

void TextureMaterial::Init(
    const char* shaderName,
    const char* diffuseName,
    const char* normalName,
    const char* specularName,
    f32 shininess)
{
    Material::Init(shaderName);

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

    if (TextureManager::Get()->Contains(diffuseName))
    {
        this->diffuse = TextureManager::Get()->Get(diffuseName);
    }
    else
    {
        this->diffuse = TextureManager::Get()->Get("DefaultMaterial_Diffuse");
    }

    if (TextureManager::Get()->Contains(normalName))
    {
        this->normal = TextureManager::Get()->Get(normalName);
    }
    else
    {
        this->normal = TextureManager::Get()->Get("DefaultMaterial_Normal");
    }

    if (TextureManager::Get()->Contains(specularName))
    {
        this->specular = TextureManager::Get()->Get(specularName);
    }
    else
    {
        this->specular = TextureManager::Get()->Get("DefaultMaterial_Specular");
    }

    this->shininess = shininess;

    ubo.shininess = this->shininess;
    if (instanceCount == 0)
    {
        uniformBuffer = GraphicsManager::Get()->UniformBufferAlloc(BIND_TO_PS, &ubo, sizeof(ubo), 0);
    }
    instanceCount++;

}

void TextureMaterial::Terminate()
{
    Material::Terminate();
    this->diffuse = nullptr;
    this->normal = nullptr;
    this->specular = nullptr;

    instanceCount--;
    ASSERT(instanceCount >= 0);
    if (instanceCount == 0)
    {
        GraphicsManager::Get()->UniformBufferFree(uniformBuffer);
    }
}

void TextureMaterial::Bind()
{
    GraphicsManager::Get()->FragmentShaderBind(shader);

    ubo.shininess = this->shininess;
    GraphicsManager::Get()->UniformBufferUpdate(uniformBuffer, &ubo);

    GraphicsManager::Get()->TextureBind(diffuse, 0);
    GraphicsManager::Get()->TextureBind(normal, 1);
    GraphicsManager::Get()->TextureBind(specular, 2);
    GraphicsManager::Get()->UniformBufferBind(uniformBuffer);
}