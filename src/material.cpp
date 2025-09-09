#include "material.h"

#include <asset_managers/shader_manager.h>
#include <asset_managers/texture_manager.h>

#include <string.h>


void Material::Init(FragmentShader* shader)
{
    this->shader = shader;
}

void Material::Terminate()
{
    this->shader = nullptr;
}

i32 SolidColorMaterial::instanceCount = 0;
UniformBuffer* SolidColorMaterial::uniformBuffer = nullptr;

void SolidColorMaterial::Init(FragmentShader* shader, const Vector3& ambient,
    const Vector3& diffuse, const Vector3& specular, f32 shininess)
{
    Material::Init(shader);
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

void TextureMaterial::Init(FragmentShader* shader, Texture* diffuse,
    Texture* normal, Texture* specular, f32 shininess)
{
    Material::Init(shader);


    if (diffuse)
    {
        this->diffuse = diffuse;
    }
    else
    {
        this->diffuse = TextureManager::Get()->Get("DefaultMaterial_Diffuse");
    }

    if (normal)
    {
        this->normal = normal;
    }
    else
    {
        this->normal = TextureManager::Get()->Get("DefaultMaterial_Normal");
    }

    if (specular)
    {
        this->specular = specular;
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

i32 PortalMaterial::instanceCount = 0;
UniformBuffer* PortalMaterial::uniformBuffer = nullptr;

void PortalMaterial::Init(FragmentShader* shader, Texture* diffuse, Texture* noise)
{
    Material::Init(shader);
    if (diffuse)
    {
        this->diffuse = diffuse;
    }
    else
    {
        this->diffuse = TextureManager::Get()->Get("DefaultMaterial_Diffuse");
    }

    if (noise)
    {
        this->noise = noise;
    }
    else
    {
        this->noise = TextureManager::Get()->Get("DefaultMaterial_Noise");
    }

    ubo.time = 0;
    if (instanceCount == 0)
    {
        uniformBuffer = GraphicsManager::Get()->UniformBufferAlloc(BIND_TO_PS, &ubo, sizeof(ubo), 0);
    }
    instanceCount++;
}

void PortalMaterial::Terminate()
{
    Material::Terminate();
    this->diffuse = nullptr;
    this->noise = nullptr;

    instanceCount--;
    ASSERT(instanceCount >= 0);
    if (instanceCount == 0)
    {
        GraphicsManager::Get()->UniformBufferFree(uniformBuffer);
    }
}

void PortalMaterial::Update(float deltaTime)
{
    ubo.time += deltaTime;
}

void PortalMaterial::Bind() 
{
    GraphicsManager::Get()->FragmentShaderBind(shader);
    GraphicsManager::Get()->UniformBufferUpdate(uniformBuffer, &ubo);
    GraphicsManager::Get()->UniformBufferBind(uniformBuffer);
    GraphicsManager::Get()->TextureBind(diffuse, 0);
    GraphicsManager::Get()->TextureBind(noise, 1);
}