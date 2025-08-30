#pragma once

#include "particle_system.h"

class BloodParticleSystem : public ParticleSystem
{
private:
    void OnParticleSpawn(Particle& particle, const Vector3& viewPos) override;
    void OnParticleUpdate(Particle& particle, const Vector3& viewPos, float deltaTime) override;
};