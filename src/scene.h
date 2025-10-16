#pragma once

#include <math/vector3.h>
#include <math/matrix4.h>
#include <math/quaternion.h>

class ActorManager;

class Scene
{
private:
    ActorManager* actorManager = nullptr;

    void LoadAssets();
    void CreateFenceActors(const char* fenceActorFile, const char* fenceInstancesFile);
    void CreateEffectActors();
    void CreatePortalActors(const char* portalActorFile, const char* portalInstancesFile);
    void CreateEnemyActors(const char* enemyActorFile, const char* enemyInstancesFile);
    void CreateEndTriggerActors(const char* endTriggerInstancesFile);
    void CreateDeadTriggerActors(const char* deadTriggerInstancesFile);
    void CreateAmmoActors(const char* ammoActorFile, const char* ammoInstancesFile);
    void CreateHealActors(const char* healActorFile, const char* healInstancesFile);
    void CreateLights(const char* lightsInstancesFile);


public:
    void Load(ActorManager* actorManager, const char *filepath);
    void Unload();
};

