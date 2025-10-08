#pragma once

#include <graphics_manager.h>
#include <math/vector3.h>
#include <stdlib.h>

class Material
{
protected:
    FragmentShader* shader = nullptr;

    Material() {}
    void Init(FragmentShader* shader);
public:
    virtual ~Material() {}
    virtual void Bind() = 0;
    virtual void Update(float dt) {};
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
    void Init(FragmentShader* shader, const Vector3& ambient,
        const Vector3& diffuse, const Vector3& specular, f32 shininess);
    void Terminate() override;
    void Bind() override;

    void SetAmbient(const Vector3& ambient);
    void SetDiffuse(const Vector3& diffuse);
    void SetSpecular(const Vector3& specular);
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
    void Init(FragmentShader* shader, Texture* diffuse, 
        Texture* normal, Texture* specular, f32 shininess);
    void Terminate() override;
    void Bind() override;
private:
    static i32 instanceCount;
    static UniformBuffer* uniformBuffer;

    Texture* diffuse = nullptr;
    Texture* specular = nullptr;
    Texture* normal = nullptr;
    f32 shininess = 0.0f;
    TextureUbo ubo;
};

struct PortalUbo
{
    float time;
    Vector3 pad;
};

class PortalMaterial : public Material
{
public:
    void Init(FragmentShader* shader, Texture* diffuse, Texture* noise);
    void Terminate() override;
    void Bind() override;
    void Update(float deltaTime) override;
private:
    static i32 instanceCount;
    static UniformBuffer* uniformBuffer;

    Texture* diffuse = nullptr;
    Texture* noise = nullptr;

    PortalUbo ubo;
};