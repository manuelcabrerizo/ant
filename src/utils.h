#pragma once

#include <math/vector3.h>
#include <math/matrix4.h>
#include <math/quaternion.h>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>

#include <common.h>

class Utils
{
public:
    static Assimp::Importer importer;
    static Matrix4 FromAssimp(aiMatrix4x4 m);
    static Vector3 FromAssimp(aiVector3D m);
    static Quaternion FromAssimp(aiQuaternion m);
    static i32 NextPower2(u32 x);
    static float Clamp(float n, float min, float max);
    static void Swap(float& a, float& b);
};