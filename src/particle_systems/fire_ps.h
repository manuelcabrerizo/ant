#pragma once

#include "particle_system.h"

class FireParticleSystem : public ParticleSystem
{
    Vector3 min, max;
private:
    void OnParticleSpawn(Particle& particle, const Vector3& viewPos) override;
    void OnParticlesUpdate(StaticArray<Particle, 500>& particles, const Vector3& viewPos, float deltaTime) override;
public:
    void SetSpawnArea(const Vector3& min, const Vector3& max);
};