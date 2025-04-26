#pragma once

struct PerDrawUbo
{
     mat4 model;
};

class RenderComponent : public Component<RenderComponent>
{
private:
    static UniformBuffer *uniformBuffer;
    static PerDrawUbo ubo;
public:
    
    TransformComponent *transform;

    Texture *texture;
    Model *model;

     // use to init static variables, must be called by the user at init time
     static void Initialize();
     static void Terminate();

     // Component logic interface, this are call form the ActorManager automaticaly
     void OnInit(ActorManager *actorManager);
     void OnTerminate(ActorManager *actorManager);
     void OnRender(ShaderManager *shaderManager, ActorManager *actorManager);
};