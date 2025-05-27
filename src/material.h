#pragma once

#include <graphics_manager.h>
#include <math/vector3.h>
#include <stdlib.h>

class FragmentShaderManager;
class TextureManager;

class Material
{
protected:
    char shaderName[_MAX_PATH + 1] = {};
    FragmentShader* shader = nullptr;

    Material() {}
    void Init(const char* shaderName, FragmentShaderManager* shaderManager);
public:
    virtual void Bind() = 0;
    virtual void Terminate();
};

struct SolidColorUbo
{
    Vector3 ambient;
    f32 pad0 = 0.0f;
    Vector3 diffuse;
    f32 pad1 = 0.0f;
    Vector3 specular;
    f32 shininess = 0.0f;
};

class SolidColorMaterial : public Material
{
public:
    void Init(const char* shaderName, FragmentShaderManager* shaderManager,
              const Vector3& ambient,
              const Vector3& diffuse,
              const Vector3& specular,
              f32 shininess);
    void Terminate() override;
    void Bind() override;
private:
    static i32 instanceCount;
    static UniformBuffer* uniformBuffer;

    Vector3 ambient = Vector3(0.2f, 0.2f, 0.2f);
    Vector3 diffuse = Vector3(0.8f, 0.8f, 0.8f);
    Vector3 specular = Vector3(1.0f, 1.0f, 1.0f);
    f32 shininess = 0.0f;
    SolidColorUbo ubo;
};

struct TextureUbo
{
    f32 shininess = 0.0f;
    Vector3 pad;
};


class TextureMaterial : public Material
{
public:
    void Init(const char* shaderName, FragmentShaderManager* shaderManager,
              const char* diffuseName,
              const char* normalName,
              const char* specularName,
              f32 shininess, 
              TextureManager* textureManager);
    void Terminate() override;
    void Bind() override;
private:
    static i32 instanceCount;
    static UniformBuffer* uniformBuffer;

    char diffuseName[_MAX_PATH + 1] = {};
    char normalName[_MAX_PATH + 1] = {};
    char specularName[_MAX_PATH + 1] = {};

    Texture* diffuse = nullptr;
    Texture* specular = nullptr;
    Texture* normal = nullptr;
    f32 shininess = 0.0f;
    TextureUbo ubo;
};