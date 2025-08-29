#include "particle_system.h"
#include <graphics_manager.h>
#include <asset_managers/shader_manager.h>
#include <asset_managers/texture_manager.h>
#include <utils.h>

void ParticleSystem::Init(int maxParticleCount, const char* texture, int memoryType)
{
    batch = GraphicsManager::Get()->BatchRendererAlloc(
        VertexShaderManager::Get()->Get("particle_vert"),
        FragmentShaderManager::Get()->Get("particle_frag"),
        TextureManager::Get()->Get(texture));
    particles.Init(maxParticleCount, memoryType);
}

void ParticleSystem::Terminate()
{
    GraphicsManager::Get()->BatchRendererFree(batch);
}

void ParticleSystem::Update(const Vector3& viewPos, float  deltaTime)
{
    for (int i = particles.size - 1; i >= 0; i--)
    {
        if (particles[i].lifeTime <= 0.0f)
        {
            particles[i] = particles[particles.size - 1];
            particles.size--;
        }
        particles[i].lifeTime -= deltaTime;
    }
}

void ParticleSystem::Render()
{
    for (int i = 0; i < particles.size; i++)
    {
        const Particle& particle = particles[i];
        batch->DrawQuad(particle.position, particle.size, particle.rotation, Vector3(0, 0, 0));
    }
    batch->Present();
}



void BloodParticleSystem::Play()
{
    isPlaying = true;
}

void BloodParticleSystem::Stop()
{
    isPlaying = false;
}

void BloodParticleSystem::Update(const Vector3& viewPos, float deltaTime)
{
    ParticleSystem::Update(viewPos, deltaTime);
    if (isPlaying)
    {
        if (timer > timeToSpawn)
        {
            SpawnParticle(viewPos);
            timer = 0.0f;
        }
        timer += deltaTime;
    }
    for (int i = 0; i < particles.size; i++)
    {
        ProcessParticle(particles[i], viewPos, deltaTime);
    }
}

void BloodParticleSystem::SpawnParticle(const Vector3& viewPos)
{
    if (particles.size + 1 <= particles.capacity)
    {
        Particle particle;
        particle.position = position + Vector3(Utils::RandRange(0.1, 0.25), Utils::RandRange(0.1, 0.25), Utils::RandRange(0.1, 0.25));
        particle.size = Vector3(Utils::RandRange(0.15, 0.25), Utils::RandRange(0.15, 0.25), 1);

        Vector3 front = (viewPos - particle.position);
        front.y = 0.0f;
        front.Normalize();
        Quaternion rotation = Quaternion::LookRotation(front, Vector3::up);

        particle.rotation = Quaternion();

        particle.velocity = Vector3(Utils::RandRange(-1.0, 1.0), Utils::RandRange(2.5, 3), Utils::RandRange(-1.0, 1.0));
        particle.lifeTime = 2.0f;
        particles.Push(particle);
    }
}

void BloodParticleSystem::ProcessParticle(Particle& particle, const Vector3& viewPos, float deltaTime)
{
    Vector3 gravityForce = Vector3(0.0f, -9.8f, 0.0f);
    particle.velocity += gravityForce * deltaTime;
    particle.position += particle.velocity * deltaTime;

    particle.size.x += deltaTime * 0.25f;
    particle.size.y += deltaTime * 0.25f;

    Vector3 front = (viewPos - particle.position);
    front.y = 0.0f;
    front.Normalize();
    particle.rotation = Quaternion::LookRotation(front, Vector3::up);
}