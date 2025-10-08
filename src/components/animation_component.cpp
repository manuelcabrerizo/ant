#include "animation_component.h"

void AnimationComponent::OnInit(ActorManager* actorManager)
{
    for (i32 i = 0; i < 100; ++i) {
        finalBoneMatrices[i] = Matrix4(1.0f);
    }
    deltaTime = 0.0f;
}

void AnimationComponent::OnUpdate(ActorManager* actorManager, f32 dt)
{
    if (isTransitioning)
    {
        AnimateTransition(dt);

        t = timer / timeToTarget;
        if (timer > timeToTarget)
        {
            current = next;
            next = -1;
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

void AnimationComponent::AddAnimation(int animationID, Animation* animation,
    bool isLooping, float speed)
{
    ASSERT(animationID < MAX_ANIMATION_STATE_COUNT);
    AnimationNode handle;
    handle.id = animationID;
    handle.animation = animation;
    handle.isLooping = isLooping;
    handle.speed = speed;
    animations.Push(handle);
}

void AnimationComponent::Animate(f32 dt)
{
    if (AnimationNode* current = GetCurrentAnimation())
    {
        deltaTime = dt;
        current->time += current->animation->GetTicksPerSeconds() * current->speed * dt;
        if (current->isLooping)
        {
            current->time = fmod(current->time, current->animation->GetDuration());
        }
        else
        {
            current->time = std::min(current->time, current->animation->GetDuration());
        }
        skeleton->CalculateBoneTransform(current->animation, skeleton->GetRoot(), Matrix4(1.0f),
            finalBoneMatrices, current->time);
    }
}

void AnimationComponent::AnimateTransition(float dt)
{
    AnimationNode* current = GetCurrentAnimation();
    AnimationNode* next = GetNextAnimation();
    if (current && next)
    {
        deltaTime = dt;
        current->time += current->animation->GetTicksPerSeconds() * current->speed * dt;
        if (current->isLooping)
        {
            current->time = fmod(current->time, current->animation->GetDuration());
        }
        else
        {
            current->time = std::min(current->time, current->animation->GetDuration());
        }

        next->time += next->animation->GetTicksPerSeconds() * next->speed * dt;
        if (current->isLooping)
        {
            next->time = fmod(next->time, next->animation->GetDuration());
        }
        else
        {
            next->time = std::min(next->time, next->animation->GetDuration());
        }


        skeleton->CalculateBoneTransform(current->animation, next->animation, skeleton->GetRoot(), Matrix4(1.0f),
            finalBoneMatrices, current->time, next->time, t);
    }
}

void AnimationComponent::Play()
{
    if (AnimationNode* current = GetCurrentAnimation())
    {
        isPlaying = true;
        current->time = 0.0f;
    }
}

void AnimationComponent::Stop()
{
    isPlaying = false;
}

bool AnimationComponent::IsPlaying()
{
    return isPlaying;
}

bool AnimationComponent::IsFinish()
{
    AnimationNode* node = GetCurrentAnimation();
    if (node->isLooping)
    {
        return true;
    }
    else
    {
        return node->time >= node->animation->GetDuration();
    }
}

bool AnimationComponent::IsTransitioning()
{
    return isTransitioning;
}

void AnimationComponent::Transition(int to, float timeToTarget)
{
    isTransitioning = true;
    timer = 0.0f;
    t = 0.0f;
    next = to;
    if (AnimationNode* nextNode = GetNextAnimation())
    {
        nextNode->time = 0.0f;
    }
    this->timeToTarget = timeToTarget;
}

AnimationNode* AnimationComponent::GetCurrentAnimation()
{
    AnimationNode* anim = nullptr;
    if (current >= 0)
    {
        for (int i = 0; i < animations.size; i++)
        {
            if (current == i)
            {
                anim = &animations[i];
                break;
            }
        }
    }
    return anim;
}

AnimationNode* AnimationComponent::GetNextAnimation()
{
    AnimationNode* anim = nullptr;
    if (next >= 0)
    {
        for (int i = 0; i < animations.size; i++)
        {
            if (next == i)
            {
                anim = &animations[i];
                break;
            }
        }
    }
    return anim;
}