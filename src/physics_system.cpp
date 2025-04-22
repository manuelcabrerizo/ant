void PhysicsSystem::Init()
{
    collisionWorld.LoadFromFile("../data/collision/level-collision.obj");
}

void PhysicsSystem::Terminate()
{

}

void PhysicsSystem::Update(ActorManager *actorManager, f32 dt)
{
    Array<PhysicsComponent>& physicsComponents = actorManager->GetComponents<PhysicsComponent>();
    for(u32 i = 0; i < physicsComponents.size; ++i)
    {
        SlotmapKey<Actor> actor = physicsComponents[i].owner;
        transform = actorManager->GetComponent<TransformComponent>(actor);
        physics = actorManager->GetComponent<PhysicsComponent>(actor);
        ProcessPhysics(dt);
        ProcessColisionDetectionAndResolution();
    }
}

void PhysicsSystem::ProcessPhysics(float dt)
{
    // Add the force of gravity
    physics->forceAccumulator += vec3(0.0f, -9.8f*3.0f, 0.0f);
    // Equations of motion
    physics->lastFrameAcceleration = physics->acceleration;
    physics->lastFrameAcceleration += physics->forceAccumulator;
    physics->velocity += physics->lastFrameAcceleration * dt;
    physics->velocity *= powf(0.01f, dt);
    transform->position += physics->velocity * dt;
    physics->forceAccumulator = vec3(0.0f);
}

void PhysicsSystem::ProcessColisionDetectionAndResolution()
{
    f32 colliderRadius = 0.5f;

    // Grounded test
    Segment groundSegment;
    vec3 colliderPos = transform->position + physics->offset;
    groundSegment.Init(colliderPos, colliderPos - vec3(0.0f, colliderRadius + 0.05f, 0.0f));
    float tOut; vec3 nOut;
    physics->grounded = collisionWorld.Intersect(groundSegment, tOut, nOut);

    // Colission detection and resolution
    Frame frame = MemoryManager::Get()->GetFrame();
    Array<CollisionData> collisionData;
    collisionData.Init(MAX_COLLISION_COUNT, FRAME_MEMORY);

    Sphere sphere;
    sphere.Init(transform->position + physics->offset, colliderRadius);
    if(collisionWorld.Intersect(sphere, collisionData))
    {
         while(collisionData.size > 0)
         {
              vec3 n = collisionData[0].n;
              f32 penetration = collisionData[0].penetration;
              transform->position += penetration * n;
              transform->position += 0.001f * n;
              physics->acceleration = vec3(0.0f);
              physics->velocity -= dot(physics->velocity, n) * n;

              collisionData.Clear();

              sphere.Init(transform->position + physics->offset, colliderRadius);
              collisionWorld.Intersect(sphere, collisionData);                    
         }    
    }
    MemoryManager::Get()->ReleaseFrame(frame);
}