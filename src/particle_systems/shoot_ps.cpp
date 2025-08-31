#include "shoot_ps.h"
#include <utils.h>
#include <math/algebra.h>

void ShootParticleSystem::OnParticleSpawn(Particle& particle, const Vector3& viewPos)
{
    Vector3 particlePos = GetPosition();
    Vector3 front = (viewPos - particlePos);
    front.y = 0.0f;
    front.Normalize();

    particle.position = particlePos;
    particle.size = Vector3(Utils::RandRange(0.05, 0.25), Utils::RandRange(0.05, 0.25), 1);
    particle.offsetRot = Quaternion::AngleAxis(Utils::RandRange(0, ANT_TAU), Vector3::forward);
    particle.rotation = particle.offsetRot * Quaternion::LookRotation(front, Vector3::up);
    particle.velocity = Vector3::zero;
    particle.lifeTime = 0.05f;
    particle.color = Vector3(1, 1, 1);
}

void ShootParticleSystem::OnParticleUpdate(Particle& particle, const Vector3& viewPos, float deltaTime)
{
    particle.position = GetPosition();
    particle.size.x += deltaTime * 0.25f;
    particle.size.y += deltaTime * 0.25f;
}