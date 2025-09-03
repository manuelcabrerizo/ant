#include "animation_component.h"

void AnimationComponent::OnInit(ActorManager* actorManager)
{
    for (i32 i = 0; i < 100; ++i) {
        finalBoneMatrices[i] = Matrix4(1.0f);
    }
    deltaTime = 0.0f;
    currentTime = 0.0f;
}

void AnimationComponent::OnUpdate(ActorManager* actorManager, f32 dt)
{
    if (isTransitioning)
    {
        t = timer / timeToTarget;
        if (timer > timeToTarget)
        {
            isTransitioning = false;
        }
        timer += dt;
    }
    else
    {
        Animate(dt);
    }
}

void AnimationComponent::SetSkeleton(Skeleton* skeleton)
{
    this->skeleton = skeleton;
}

void AnimationComponent::SetAnimation(int animationID)
{
    this->current = animationID;
}

void AnimationComponent::AddAnimation(int animationID, Animation* animation)
{
    ASSERT(animationID < MAX_ANIMATION_STATE_COUNT);
    AnimationNode handle;
    handle.id = animationID;
    handle.animation = animation;
    animations.Push(handle);
}

void AnimationComponent::Animate(f32 dt)
{
    if (Animation* current = GetCurrentAnimation())
    {
        deltaTime = dt;
        currentTime += current->GetTicksPerSeconds() * dt;
        currentTime = fmod(currentTime, current->GetDuration());
        skeleton->CalculateBoneTransform(current, skeleton->GetRoot(), Matrix4(1.0f),
            finalBoneMatrices, currentTime);
    }
}

void AnimationComponent::Play()
{
    isPlaying = true;
}

void AnimationComponent::Stop()
{
    isPlaying = false;
}

bool AnimationComponent::IsPlaying()
{
    return isPlaying;
}

void AnimationComponent::Transition(int to, float timeToTarget)
{
    isTransitioning = true;
    timer = 0.0f;
    t = 0.0f;
}

Animation* AnimationComponent::GetCurrentAnimation()
{
    Animation* anim = nullptr;
    if (current >= 0)
    {
        for (int i = 0; i < animations.size; i++)
        {
            if (current == i)
            {
                anim = animations[i].animation;
                break;
            }
        }
    }
    return anim;
}

Animation* AnimationComponent::GetNextAnimation()
{
    Animation* anim = nullptr;
    if (next >= 0)
    {
        for (int i = 0; i < animations.size; i++)
        {
            if (next == i)
            {
                anim = animations[i].animation;
                break;
            }
        }
    }
    return anim;
}