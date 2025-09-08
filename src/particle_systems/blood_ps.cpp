#include "blood_ps.h"
#include <utils.h>
#include <math/algebra.h>

void BloodParticleSystem::OnParticleSpawn(Particle& particle, const Vector3& viewPos)
{
    Vector3 particlePos = GetPosition();
    Vector3 front = (viewPos - particlePos).Normalized();
    Vector3 right = Vector3::Cross(Vector3::up, front).Normalized();
    Vector3 up = Vector3::Cross(front, right);
    Matrix4 rotMat = Matrix4::TransformFromBasis(Vector3::zero, right, up, front);

    particle.position = particlePos;
    particle.size = Vector3(Utils::RandRange(0.15, 0.25), Utils::RandRange(0.15, 0.25), 1);
    particle.offsetRot = Quaternion::AngleAxis(Utils::RandRange(0, ANT_TAU), Vector3::forward);
    particle.rotation = particle.offsetRot.ToMatrix4() * rotMat;
    particle.velocity = Vector3(Utils::RandRange(-1.0, 1.0), Utils::RandRange(3.5, 4), Utils::RandRange(-1.0, 1.0));
    particle.lifeTime = 2.0f;
    particle.color = Vector3(Utils::RandRange(0.1, 1), 1, 1);
}

void BloodParticleSystem::OnParticlesUpdate(StaticArray<Particle, 500>& particles, const Vector3& viewPos, float deltaTime)
{
    Vector3 gravityForce = Vector3(0.0f, -9.8f * 1.5f, 0.0f);
    for (int i = 0; i < particles.size; i++)
    {
        Particle& particle = particles[i];
        particle.velocity += gravityForce * deltaTime;
        particle.position += particle.velocity * deltaTime;
        particle.size.x += deltaTime * 0.5f;
        particle.size.y += deltaTime * 0.5f;
        Vector3 front = (viewPos - particle.position).Normalized();
        Vector3 right = Vector3::Cross(Vector3::up, front).Normalized();
        Vector3 up = Vector3::Cross(front, right);
        Matrix4 rotMat = Matrix4::TransformFromBasis(Vector3::zero, right, up, front);
        particle.rotation = particle.offsetRot.ToMatrix4() * rotMat;
    }
}