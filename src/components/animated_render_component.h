#pragma once

#include "component.h"
#include <math/vector3.h>
#include <math/matrix4.h>

struct UniformBuffer;
class TransformComponent;
class AnimationComponent;
class Model;

class AnimatedRenderComponent : public Component<AnimatedRenderComponent>
{
private:
    struct PerDrawUbo
    {
        Matrix4 model;
    };

    static UniformBuffer* uniformBuffer;
    static PerDrawUbo ubo;
    static UniformBuffer* matrixBuffer;

    AnimationComponent* animation = nullptr;
public:
    Model* model;
    Vector3 rotationOffset;

    // use to init static variables, must be called by the user at init time
    static void Initialize();
    static void Terminate();

    // Component logic interface, this are call form the ActorManager automaticaly     
    void OnInit(ActorManager* actorManager);
    void OnTerminate(ActorManager* actorManager) override;

    static void OnSetRenderState();
    void OnRender(ActorManager* actorManager);
};