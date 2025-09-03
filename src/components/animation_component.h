#pragma once
#include "component.h"
#include "animation/skeleton.h"
#include "animation/animation.h"
#include <containers.h>
#include <math\matrix4.h>

#define MAX_ANIMATION_STATE_COUNT 10

struct AnimationNode
{
    Animation* animation = nullptr;
    int id;
    float time = 0.0f;
    float speed = 1.0f;
    bool isLooping;
};

class AnimationComponent : public Component<AnimationComponent>
{
private:
    Skeleton *skeleton = nullptr;
    StaticArray<AnimationNode, MAX_ANIMATION_STATE_COUNT> animations;

    int current = -1;
    int next = -1;
    float timeToTarget = 0.0f;
    float timer = 0.0f;
    float t = 0.0f;

    bool isPlaying = false;
    bool isTransitioning = false;

    Matrix4 finalBoneMatrices[100];
    f32 deltaTime = 0.0f;

public:
    void OnInit(ActorManager* actorManager) override;
    void OnUpdate(ActorManager* actorManager, f32 dt);

    void SetSkeleton(Skeleton* skeleton);
    void SetAnimation(int animationID);
    void AddAnimation(int animationID, Animation* animation, bool isLooping);

    void Animate(f32 dt);
    void AnimateTransition(float dt);

    void Play();
    void Stop();
    bool IsPlaying(); 
    void Transition(int to, float timeToTarget);

    AnimationNode* GetCurrentAnimation();
    AnimationNode* GetNextAnimation();

    Matrix4* GetMatrices()
    {
        return finalBoneMatrices;
    }

};