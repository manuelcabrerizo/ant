
void CameraComponent::Init(vec3 pos, vec3 dir)
{
     worldUp = vec3(0.0f, 1.0f, 0.0f);
     
     position = pos;
     front = normalize(dir);
     right = normalize(cross(worldUp, front));
     up = cross(front, right);
     // TODO: this should never be zero
     worldFront = normalize(vec3(front.x, 0.0f, front.z));
}

void CameraComponent::SetPosition(vec3 pos)
{
     position = pos;
}

void CameraComponent::SetDirection(vec3 dir)
{
     front = normalize(dir);
     right = normalize(cross(worldUp, front));
     up = cross(front, right);
     // TODO: this should never be zero
     worldFront = normalize(vec3(front.x, 0.0f, front.z));
}

vec3 CameraComponent::GetPosition()
{
     return position;
}

vec3 CameraComponent::GetFront()
{
     return front;
}

vec3 CameraComponent::GetRight()
{
     return right;
}

vec3 CameraComponent::GetUp()
{
     return up;
}

vec3 CameraComponent::GetWorldFront()
{
     return worldFront;
}

vec3 CameraComponent::GetWorldUp()
{
     return worldUp;
}


mat4 CameraComponent::GetView()
{
     return lookAt(position, position + front, worldUp);
}
