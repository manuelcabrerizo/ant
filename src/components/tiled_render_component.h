#pragma once

#include "component.h"
#include <math/vector3.h>
#include <math/matrix4.h>

struct UniformBuffer;
class TransformComponent;
class Model;

class TiledRenderComponent : public Component<TiledRenderComponent>
{
private:
    struct PerDrawUbo
    {
        Matrix4 model;
        float width;
        float height;
        float pad0;
        float pad1;
    };

    static UniformBuffer* uniformBuffer;
    static PerDrawUbo ubo;

    TransformComponent* transform = nullptr;

    Model* model = nullptr;
    Vector3 rotationOffset;

public:
    static void Initialize();
    static void Terminate();
    static void OnSetRenderState();

    void OnInit(ActorManager* actorManager);
    void OnTerminate(ActorManager* actorManager) override;
    void OnRender(ActorManager* actorManager);

    void SetModel(Model* model);
    Model* GetModel() const;
    void SetRotationOffset(const Vector3& offset);
    Vector3 GetRotationOffset();
};