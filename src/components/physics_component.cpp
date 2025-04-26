CollisionWorld PhysicsComponent::collisionWorld;

void PhysicsComponent::Initialize()
{
    collisionWorld.LoadFromFile("../data/collision/level-collision.obj");
}

void PhysicsComponent::Terminate()
{

}

void PhysicsComponent::OnInit(ActorManager *actorManager)
{
    transform = actorManager->GetComponent<TransformComponent>(owner);
}

void PhysicsComponent::OnTerminate(ActorManager *actorManager)
{

}

void PhysicsComponent::OnUpdate(ActorManager *actorManager, f32 dt)
{
    ProcessPhysics(dt);
    ProcessColisionDetectionAndResolution();
}

void PhysicsComponent::ProcessPhysics(float dt)
{
    // Add the force of gravity
    forceAccumulator += vec3(0.0f, -9.8f*3.0f, 0.0f);
    // Equations of motion
    lastFrameAcceleration = acceleration;
    lastFrameAcceleration += forceAccumulator;
    velocity += lastFrameAcceleration * dt;
    velocity *= powf(0.01f, dt);
    transform->position += velocity * dt;
    forceAccumulator = vec3(0.0f);
}

void PhysicsComponent::ProcessColisionDetectionAndResolution()
{
    f32 colliderRadius = 0.5f;

    // Grounded test
    Segment groundSegment;
    vec3 colliderPos = transform->position + offset;
    groundSegment.Init(colliderPos, colliderPos - vec3(0.0f, colliderRadius + 0.05f, 0.0f));
    float tOut; vec3 nOut;
    grounded = collisionWorld.Intersect(groundSegment, tOut, nOut);

    // Colission detection and resolution
    Frame frame = MemoryManager::Get()->GetFrame();
    Array<CollisionData> collisionData;
    collisionData.Init(MAX_COLLISION_COUNT, FRAME_MEMORY);

    Sphere sphere;
    sphere.Init(transform->position + offset, colliderRadius);
    if(collisionWorld.Intersect(sphere, collisionData))
    {
         while(collisionData.size > 0)
         {
              vec3 n = collisionData[0].n;
              f32 penetration = collisionData[0].penetration;
              transform->position += penetration * n;
              transform->position += 0.001f * n;
              acceleration = vec3(0.0f);
              velocity -= dot(velocity, n) * n;

              collisionData.Clear();

              sphere.Init(transform->position + offset, colliderRadius);
              collisionWorld.Intersect(sphere, collisionData);                    
         }    
    }
    MemoryManager::Get()->ReleaseFrame(frame);
}