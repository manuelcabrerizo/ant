#pragma once

#include <containers.h>
#include <math\vector3.h>
#include <math\quaternion.h>

class BatchRenderer;

// the particle system should not be deleted during gameplay
// it should be reuse when posible
struct Particle
{
    Vector3 position;
    Vector3 velocity;
    Vector3 size;
    Quaternion rotation;

    float lifeTime;
};

class ParticleSystem
{
private:
    BatchRenderer* batch;
protected:
    Vector3 position;
    Array<Particle> particles;
public:
    virtual ~ParticleSystem() {};
    void Init(int maxParticleCount, const char *texture, int memoryType);
    void Terminate();
    virtual void Update(const Vector3& viewPos, float  deltaTime);
    void Render();

    void SetPosition(const Vector3& position)
    {
        this->position = position;
    }
};

class BloodParticleSystem : public ParticleSystem
{
private: 
    bool isPlaying = false;
    float timeToSpawn = 0.01f;
    float timer = 0.0f;

    void SpawnParticle(const Vector3& viewPos);
    void ProcessParticle(Particle& particle, const Vector3& viewPos, float deltaTime);
public:
    void Play();
    void Stop();
    void Update(const Vector3& viewPos, float deltaTime) override;
};