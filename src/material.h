#pragma once

#include <graphics_manager.h>
#include <math/vector3.h>
#include <stdlib.h>

class ShaderManager;
class TextureManager;

class Material
{
private:
    char shaderName[_MAX_PATH + 1] = {};
protected:
    void Init(const char* shaderName, ShaderManager* shaderManager);
public:
    virtual void Bind() = 0;
    virtual void Terminate();

    Shader* shader = nullptr;
};

class SolidColorMaterial : public Material
{
public:
    void Init(const char* shaderName, ShaderManager* shaderManager,
              const Vector3& ambient,
              const Vector3& diffuse,
              const Vector3& specular,
              f32 shininess);
    void Terminate() override;
    void Bind() override;
private:
    Vector3 ambient = Vector3(0.2f, 0.2f, 0.2f);
    Vector3 diffuse = Vector3(0.8f, 0.8f, 0.8f);
    Vector3 specular = Vector3(1.0f, 1.0f, 1.0f);
    f32 shininess = 0.0f;
};

class TextureMaterial : public Material
{
public:
    void Init(const char* shaderName, ShaderManager* shaderManager,
              const char* diffuseName,
              const char* normalName,
              const char* specularName,
              f32 shininess, 
              TextureManager* textureManager);
    void Terminate() override;
    void Bind() override;
private:
    char diffuseName[_MAX_PATH + 1] = {};
    char normalName[_MAX_PATH + 1] = {};
    char specularName[_MAX_PATH + 1] = {};

    Texture* diffuse = nullptr;
    Texture* specular = nullptr;
    Texture* normal = nullptr;
    f32 shininess = 0.0f;
};