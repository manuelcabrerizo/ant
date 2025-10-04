#include "shoot_ps.h"
#include <utils.h>
#include <math/algebra.h>

void ShootParticleSystem::OnParticleSpawn(Particle& particle, const Vector3& viewPos)
{
    Vector3 particlePos = GetPosition();
    
    Vector3 front = (viewPos - particlePos).Normalized();
    Vector3 right = Vector3::Cross(Vector3::up, front).Normalized();
    Vector3 up = Vector3::Cross(front, right);
    Matrix4 rotMat = Matrix4::TransformFromBasis(Vector3::zero, right, up, front);

    particle.position = particlePos;
    particle.size = Vector3(Utils::RandRange(0.05, 0.35), Utils::RandRange(0.05, 0.5), 1);
    particle.offsetRot = Quaternion::AngleAxis(Utils::RandRange(0, ANT_TAU), Vector3::forward);
    particle.rotation = particle.offsetRot.ToMatrix4() * rotMat;
    particle.velocity = Vector3::zero;
    particle.lifeTime = 0.05f;
    particle.color = Vector3(2);
}

void ShootParticleSystem::OnParticlesUpdate(StaticArray<Particle, 500>& particles, const Vector3& viewPos, float deltaTime)
{
    for (int i = 0; i < particles.size; i++)
    {
        Particle& particle = particles[i];
        particle.position = GetPosition();
        particle.size.x += deltaTime * 0.25f;
        particle.size.y += deltaTime * 0.25f;
    }
}