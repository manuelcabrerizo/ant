#pragma once

class Animation;

class Bone
{
    friend Animation;
    template <typename Type>
    struct Key
    {
        Type value;
        f32 timeStamp;
    };
private:
    Array<Key<Vector3>> positions;
    Array<Key<Quaternion>> rotations;
    Array<Key<Vector3>> scales;

    Matrix4 localTransform = Matrix4(1.0f);
    i32 id = 0;
    void Init(i32 id, aiNodeAnim *channel, i32 memoryType);
    template <typename Type>
    static i32 GetIndex(Array<Key<Type>> &array, f32 animationTime);
    template <typename Type>
    static Type Interpolate(Array<Key<Type>> &array, f32 animationTime);
    
public:
    void Update(f32 animationTime);
    Matrix4 GetLocalTransform();
    i32 GetId();
};

inline Matrix4 ai_mat4_to_sd_mat4(aiMatrix4x4 m);
inline Vector3 ai_vec3_to_sd_vec3(aiVector3D m);
inline Quaternion ai_quat_to_sd_quat(aiQuaternion m);
inline f32 get_scale_factor(f32 last_time_stamp, f32 next_time_stamp, f32 animation_time);
