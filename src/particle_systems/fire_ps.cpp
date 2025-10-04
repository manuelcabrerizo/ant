#include "fire_ps.h"
#include <utils.h>
#include <math/algebra.h>

void FireParticleSystem::OnParticleSpawn(Particle& particle, const Vector3& viewPos)
{
    Vector3 front = (viewPos - particle.position).Normalized();
    Vector3 right = Vector3::Cross(Vector3::up, front).Normalized();
    Vector3 up = Vector3::Cross(front, right);
    Matrix4 rotMat = Matrix4::TransformFromBasis(Vector3::zero, right, up, front);

    particle.position = GetPosition() + Vector3(Utils::RandRange(min.x, max.x), 0, Utils::RandRange(min.z, max.z));
    particle.size = Vector3(Utils::RandRange(2.0, 8.0), 2, 1);
    particle.offsetRot = Quaternion::AngleAxis(Utils::RandRange(0, ANT_TAU*2.0f), Vector3::forward);
    particle.rotation = particle.offsetRot.ToMatrix4() * rotMat;
    particle.velocity = Vector3(Utils::RandRange(-0.1, 0.1), Utils::RandRange(0.5, 1.0), Utils::RandRange(-0.1, 0.1));
    particle.lifeTime = 3.0f;
    particle.color = Vector3(1, 1, 1);
}

void FireParticleSystem::OnParticlesUpdate(StaticArray<Particle, 500>& particles, const Vector3& viewPos, float deltaTime)
{
    for (int i = 0; i < particles.size; i++)
    {
        Particle& particle = particles[i];
        particle.position += particle.velocity * deltaTime;
        particle.size.x += deltaTime * 0.5f;
        particle.size.y += deltaTime * 0.5f;
        Vector3 front = (viewPos - particle.position).Normalized();
        Vector3 right = Vector3::Cross(Vector3::up, front).Normalized();
        Vector3 up = Vector3::Cross(front, right);
        Matrix4 rotMat = Matrix4::TransformFromBasis(Vector3::zero, right, up, front);
        particle.rotation = particle.offsetRot.ToMatrix4() * rotMat;
        particle.color = Vector3(10);
    }
}

void FireParticleSystem::SetSpawnArea(const Vector3& min, const Vector3& max)
{
    this->min = min;
    this->max = max;
}
