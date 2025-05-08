#include "bone.h"

//==========================================================================================
// TODO: change the name of this functions
//==========================================================================================
inline Matrix4 ai_mat4_to_sd_mat4(aiMatrix4x4 m)
{
    Matrix4 mat = Matrix4(m.a1, m.b1, m.c1, m.d1,
                          m.a2, m.b2, m.c2, m.d2, 
                          m.a3, m.b3, m.c3, m.d3, 
                          m.a4, m.b4, m.c4, m.d4);                    
    return Matrix4::Transposed(mat);
}

inline Vector3 ai_vec3_to_sd_vec3(aiVector3D m) 
{
    return Vector3(m.x, m.y, m.z);
}

inline Quaternion ai_quat_to_sd_quat(aiQuaternion m) 
{
    return Quaternion(m.w, m.x, m.y, m.z);
}

inline f32 get_scale_factor(f32 last_time_stamp, f32 next_time_stamp, f32 animation_time) 
{
    f32 scale_factor = (animation_time - last_time_stamp) / (next_time_stamp - last_time_stamp);
    return scale_factor;
}
//==========================================================================================


void Bone::Init(i32 id, aiNodeAnim *channel, i32 memoryType)
{
    this->id = id;
    localTransform = Matrix4(1.0f);
    positions.Init(channel->mNumPositionKeys, memoryType);
    rotations.Init(channel->mNumRotationKeys, memoryType);
    scales.Init(channel->mNumScalingKeys, memoryType);

    for(i32 i = 0; i < channel->mNumPositionKeys; ++i)
    {
        aiVector3D position = channel->mPositionKeys[i].mValue;
        f32 timeStamp = channel->mPositionKeys[i].mTime;
        Key<Vector3> keyPosition;
        keyPosition.value = ai_vec3_to_sd_vec3(position);
        keyPosition.timeStamp = timeStamp;
        positions.Push(keyPosition);
    }

    for(i32 i = 0; i < channel->mNumRotationKeys; ++i)
    {
        aiQuaternion rotation = channel->mRotationKeys[i].mValue;
        f32 timeStamp = channel->mRotationKeys[i].mTime;
        Key<Quaternion> keyRotation;
        keyRotation.value = ai_quat_to_sd_quat(rotation);
        keyRotation.timeStamp = timeStamp;
        rotations.Push(keyRotation);
    }

    for(i32 i = 0; i < channel->mNumScalingKeys; ++i)
    {
        aiVector3D scale = channel->mScalingKeys[i].mValue;
        f32 timeStamp = channel->mScalingKeys[i].mTime;
        Key<Vector3> keyScale;
        keyScale.value = ai_vec3_to_sd_vec3(scale);
        keyScale.timeStamp = timeStamp;
        scales.Push(keyScale);
    }
}

template <typename Type>
i32 Bone::GetIndex(Array<Key<Type>> &array, f32 animationTime)
{
    for(i32 i = 0; i < array.size - 1; ++i) 
    {
        if(animationTime < array[i + 1].timeStamp)
        {
            return i;
        }
    }
    return array.size - 2;
}

template <>
Vector3 Bone::Interpolate<Vector3>(Array<Key<Vector3>> &array, f32 animationTime)
{
    if(array.size == 1)
    {
        return array[0].value;
    }
    i32 index0 = GetIndex<Vector3>(array, animationTime);
    i32 index1 = index0 + 1;
    f32 scaleFactor = get_scale_factor(array[index0].timeStamp, array[index1].timeStamp, animationTime);
    return array[index0].value.Lerp(array[index1].value, scaleFactor);
}

template <>
Quaternion Bone::Interpolate<Quaternion>(Array<Key<Quaternion>> &array, f32 animationTime)
{
    if(array.size == 1) 
    {
        return array[0].value.Normalized();
    }
    i32 index0 = GetIndex<Quaternion>(array, animationTime);
    i32 index1 = index0 + 1;
    f32 scaleFactor = get_scale_factor(array[index0].timeStamp, array[index1].timeStamp, animationTime);
    Quaternion rot = Quaternion::Slerp(array[index0].value, array[index1].value, scaleFactor);
    return rot.Normalized();
}

void Bone::Update(f32 animationTime)
{
    Matrix4 translation = Matrix4::Translate(Interpolate<Vector3>(positions, animationTime));
    Matrix4 rotation = Interpolate<Quaternion>(rotations, animationTime).ToMatrix4();
    Matrix4 sca = Matrix4::Scale(Interpolate<Vector3>(scales, animationTime));
    localTransform = translation * rotation * sca; // TODO: change order of mul
}

Matrix4 Bone::GetLocalTransform()
{
    return localTransform;
}

i32 Bone::GetId()
{
    return id;
}