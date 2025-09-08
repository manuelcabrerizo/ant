#pragma once

#include "particle_system.h"

class BloodParticleSystem : public ParticleSystem
{
private:
    void OnParticleSpawn(Particle& particle, const Vector3& viewPos) override;
    void OnParticlesUpdate(StaticArray<Particle, 500>& particles, const Vector3& viewPos, float deltaTime) override;
};