#pragma once

#include <containers.h>
#include <math/vector3.h>
#include <math/quaternion.h>

#include <assimp/scene.h>

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

    //Matrix4 localTransform = Matrix4(1.0f);
    i32 id = 0;
    void Init(i32 id, aiNodeAnim *channel, i32 memoryType);
    template <typename Type>
    static i32 GetIndex(Array<Key<Type>> &array, f32 animationTime);
    template <typename Type>
    static Type Interpolate(Array<Key<Type>> &array, f32 animationTime);
    
public:
    Matrix4 Update(f32 animationTime);
    i32 GetId();
};

inline f32 get_scale_factor(f32 last_time_stamp, f32 next_time_stamp, f32 animation_time);
