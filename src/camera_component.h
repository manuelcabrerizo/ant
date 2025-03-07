#pragma once

struct Actor;

class CameraComponent
{
private:
     vec3 position;

     vec3 front;
     vec3 right;
     vec3 up;
     vec3 worldUp;
     vec3 worldFront;

public:
     SlotmapKey<Actor> owner;
     
     void Init(vec3 pos, vec3 dir);

     void SetPosition(vec3 pos);
     void SetDirection(vec3 dir);

     vec3 GetPosition();
     vec3 GetFront();
     vec3 GetRight();
     vec3 GetUp();
     vec3 GetWorldFront();
     vec3 GetWorldUp();

     mat4 GetView();
};
