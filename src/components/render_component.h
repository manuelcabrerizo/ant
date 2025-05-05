#pragma once

#include "animation/skeleton.h"
#include "animation/animation.h"

struct PerDrawUbo
{
     mat4 model;
};

class RenderComponent : public Component<RenderComponent>
{
private:
    static UniformBuffer *uniformBuffer;
    static PerDrawUbo ubo;

    static UniformBuffer *matrixBuffer;
    static Skeleton skeleton;
    static Animation animation;
public:
    
    TransformComponent *transform;

    Texture *texture;
    Model *model;
    bool isAnimated = false;

     // use to init static variables, must be called by the user at init time
     static void Initialize();
     static void Terminate();

     // Component logic interface, this are call form the ActorManager automaticaly
     void OnInit(ActorManager *actorManager);
     void OnTerminate(ActorManager *actorManager) override;
     void OnUpdate(ActorManager *actorManager, f32 dt);
     void OnRender(ShaderManager *shaderManager, ActorManager *actorManager);
};