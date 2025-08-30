#pragma once

#include <containers.h>
#include <math\vector3.h>
#include <math\quaternion.h>

class BatchRenderer;
class Texture;

// the particle system should not be deleted during gameplay
// it should be reuse when posible
struct Particle
{
    Vector3 position;
    Vector3 velocity;
    Vector3 size;
    Quaternion rotation;
    Quaternion offsetRot;
    Vector3 color;

    float lifeTime;
};

class ParticleSystem
{
private:
    static BatchRenderer* batch;
public:
    static void Init();
    static void Terminate();

private:
    Vector3 position;
    Texture* texture = nullptr;
    StaticArray<Particle, 500> particles;
    bool isPlaying = false;
    float timeToSpawn = 0.005f;
    float timer = 0.0f;
public:
    virtual ~ParticleSystem() {};
    void Update(const Vector3& viewPos, float  deltaTime);
    void Render();
    void Play();
    void Stop();
    Vector3 GetPosition();
    void SetPosition(const Vector3& position);
    void SetTexture(Texture* texture);
    virtual void OnParticleSpawn(Particle& particle, const Vector3& viewPos) = 0;
    virtual void OnParticleUpdate(Particle& particle, const Vector3& viewPos, float deltaTime) = 0;
};

class BloodParticleSystem : public ParticleSystem
{
private: 
    void OnParticleSpawn(Particle& particle, const Vector3& viewPos) override;
    void OnParticleUpdate(Particle& particle, const Vector3& viewPos, float deltaTime) override;
};