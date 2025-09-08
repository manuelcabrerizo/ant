#include "particle_system.h"
#include <graphics_manager.h>
#include <asset_managers/shader_manager.h>
#include <asset_managers/texture_manager.h>

BatchRenderer* ParticleSystem::batch = nullptr;

void ParticleSystem::Init()
{
    batch = GraphicsManager::Get()->BatchRendererAlloc(
        VertexShaderManager::Get()->Get("particle_vert"),
        FragmentShaderManager::Get()->Get("particle_frag"),
        TextureManager::Get()->Get("DefaultMaterial_Diffuse"));
}

void ParticleSystem::Terminate()
{
    GraphicsManager::Get()->BatchRendererFree(batch);
}

void ParticleSystem::Update(const Vector3& viewPos, float  deltaTime)
{
    // Remove dead particles
    for (int i = particles.size - 1; i >= 0; i--)
    {
        if (particles[i].lifeTime <= 0.0f)
        {
            particles[i] = particles[particles.size - 1];
            particles.size--;
        }
        particles[i].lifeTime -= deltaTime;
    }

    // Spawn new Particles
    if (isPlaying)
    {
        if (timer > timeToSpawn)
        {
            if (particles.size + 1 <= particles.GetCapacity())
            {
                Particle particle;
                OnParticleSpawn(particle, viewPos);
                particles.Push(particle);
            }
            timer = 0.0f;
        }
        timer += deltaTime;
    }

    // Update Particles
    OnParticlesUpdate(particles, viewPos, deltaTime);
}

void ParticleSystem::Render()
{
    if (particles.size == 0)
    {
        return;
    }

    if (texture)
    {
        batch->SetTexture(texture);
    }

    for (int i = 0; i < particles.size; i++)
    {
        const Particle& particle = particles[i];
        batch->DrawQuad(particle.position, particle.size, particle.rotation, particle.color);
    }
    batch->Present();
}

void ParticleSystem::Play()
{
    isPlaying = true;
}

void ParticleSystem::Stop()
{
    isPlaying = false;
}

Vector3 ParticleSystem::GetPosition()
{
    return position;
}

void ParticleSystem::SetPosition(const Vector3& position)
{
    this->position = position;
}

void ParticleSystem::SetTexture(Texture* texture)
{
    this->texture = texture;
}