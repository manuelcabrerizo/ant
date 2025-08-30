#include "particle_system.h"
#include <graphics_manager.h>
#include <asset_managers/shader_manager.h>
#include <asset_managers/texture_manager.h>
#include <utils.h>
#include <math/algebra.h>

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
    for (int i = 0; i < particles.size; i++)
    {
        OnParticleUpdate(particles[i], viewPos, deltaTime);
    }
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


// Blood Particle System Implementation
void BloodParticleSystem::OnParticleSpawn(Particle& particle, const Vector3& viewPos)
{
    Vector3 particlePos = GetPosition() + Vector3(Utils::RandRange(0.1, 0.25), Utils::RandRange(0.1, 0.25), Utils::RandRange(0.1, 0.25));
    Vector3 front = (viewPos - particlePos);
    front.y = 0.0f;
    front.Normalize();

    particle.position = particlePos;
    particle.size = Vector3(Utils::RandRange(0.15, 0.25), Utils::RandRange(0.15, 0.25), 1);
    particle.offsetRot = Quaternion::AngleAxis(Utils::RandRange(0, ANT_TAU), Vector3::forward);
    particle.rotation = particle.offsetRot * Quaternion::LookRotation(front, Vector3::up);
    particle.velocity = Vector3(Utils::RandRange(-1.0, 1.0), Utils::RandRange(3.5, 4), Utils::RandRange(-1.0, 1.0));
    particle.lifeTime = 2.0f;
    particle.color = Vector3(Utils::RandRange(0.1, 1), 1, 1);
}

void BloodParticleSystem::OnParticleUpdate(Particle& particle, const Vector3& viewPos, float deltaTime)
{
    Vector3 gravityForce = Vector3(0.0f, -9.8f*1.5f, 0.0f);
    particle.velocity += gravityForce * deltaTime;
    particle.position += particle.velocity * deltaTime;
    particle.size.x += deltaTime * 0.5f;
    particle.size.y += deltaTime * 0.5f;
    Vector3 front = (viewPos - particle.position);
    front.y = 0.0f;
    front.Normalize();
    particle.rotation = particle.offsetRot * Quaternion::LookRotation(front, Vector3::up);
}