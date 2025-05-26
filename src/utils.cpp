#include "utils.h"

Assimp::Importer Utils::importer;

Matrix4 Utils::FromAssimp(aiMatrix4x4 m)
{
    Matrix4 mat = Matrix4(m.a1, m.b1, m.c1, m.d1,
        m.a2, m.b2, m.c2, m.d2,
        m.a3, m.b3, m.c3, m.d3,
        m.a4, m.b4, m.c4, m.d4);
    return Matrix4::Transposed(mat);
}

Vector3 Utils::FromAssimp(aiVector3D m)
{
    return Vector3(m.x, m.y, m.z);
}

Quaternion Utils::FromAssimp(aiQuaternion m)
{
    return Quaternion(m.w, m.x, m.y, m.z);
}
