#include "matrix4.h"
#include <math.h>
#include <common.h>
#include "quaternion.h"

float *Matrix4::operator[](int index)
{
    return &(v[index * 4]);
}

Matrix4 Matrix4::operator+(const Matrix4 &m) const
{
    return Matrix4(
        m11 + m.m11, m12 + m.m12, m13 + m.m13, m14 + m.m14,
        m21 + m.m21, m22 + m.m22, m23 + m.m23, m24 + m.m24,
        m31 + m.m31, m32 + m.m32, m33 + m.m33, m34 + m.m34,
        m41 + m.m41, m42 + m.m42, m43 + m.m43, m44 + m.m44);
}

Matrix4 Matrix4::operator*(float val) const
{
    return Matrix4(
        m11 * val, m12 * val, m13 * val, m14 * val,
        m21 * val, m22 * val, m23 * val, m24 * val,
        m31 * val, m32 * val, m33 * val, m34 * val,
        m41 * val, m42 * val, m43 * val, m44 * val);
}

Matrix4 Matrix4::operator*(const Matrix4 &m) const
{
    Matrix4 result;
    for(int row = 0; row < 4; ++row) {
        for(int col = 0; col < 4; ++col) {
            result[row][col] =
                v[row * 4 + 0] * m.v[0 * 4 + col] +
                v[row * 4 + 1] * m.v[1 * 4 + col] +
                v[row * 4 + 2] * m.v[2 * 4 + col] +
                v[row * 4 + 3] * m.v[3 * 4 + col];
        }
    }
    return result;
}

Vector4 Matrix4::operator*(const Vector4& vec) const
{
    Vector4 result;
    for (int col = 0; col < 4; ++col) {
        result.v[col] =
            v[0 * 4 + col] * vec.x +
            v[1 * 4 + col] * vec.y +
            v[2 * 4 + col] * vec.z +
            v[3 * 4 + col] * vec.w;
    }
    return result;
}

Vector3 Matrix4::GetTranslation()
{
    return Vector3(m41, m42, m43);
}

Quaternion Matrix4::GetRotation()
{
    Quaternion q;
    float t;
    if (m22 < 0) {
        if (m11 > m22) {
            t = 1 + m11 - m22 - m33;
            q = Quaternion(t, m12 + m21, m31 + m13, m23 - m32);
        }
        else {
            t = 1 - m11 + m22 - m33;
            q = Quaternion(m12 + m21, t, m23 + m32, m31 - m13);
        }
    }
    else {
        if (m11 < -m22) {
            t = 1 - m11 - m22 + m33;
            q = Quaternion(m31 + m13, m23 + m32, t, m12 - m21);
        }
        else {
            t = 1 + m11 + m22 + m33;
            q = Quaternion(m23 - m32, m31 - m13, m12 - m21, t);
        }
    }
    q = q * (0.5f / sqrt(t));

    return q;
}

Vector3 Matrix4::GetScale()
{
    Matrix4 rotScaleMat(
        v[0], v[1], v[2], 0,
        v[4], v[5], v[6], 0,
        v[8], v[9], v[10], 0,
        0, 0, 0, 1
    );
    Matrix4 invRotMat = Quaternion::Inverse(GetRotation()).ToMatrix4();
    Matrix4 scaleSkewMat = rotScaleMat * invRotMat;
    return Vector3(scaleSkewMat.v[0], scaleSkewMat.v[5], scaleSkewMat.v[10]);
}

Vector3 Matrix4::TransformPoint(const Matrix4& mat, const Vector3 &vec)
{
    Vector4 result = mat * Vector4(vec, 1.0f);
    return Vector3(result.x, result.y, result.z);
}

Vector3 Matrix4::TransformVector(const Matrix4& mat, const Vector3 &vec)
{
    Vector4 result = mat * Vector4(vec, 0.0f);
    return Vector3(result.x, result.y, result.z);
}

Matrix4 Matrix4::Frustum(float l, float r, float b, float t, float n, float f)
{
    if (l == r || t == b || n == f) {
        ASSERT(!"WARNING: Trying to create invalid frustum\n");
    }

    Matrix4 mat = Matrix4(
        (2*n) /(r-l), 0, -(r+l)/(r-l), 0,
        0, (2*n)/(t-b), -(t+b)/(t-b), 0,
        0, 0, f/(f-n), -(f*n)/(f-n),
        0, 0, 1, 0
    );
    return Matrix4::Transposed(mat);

}

Matrix4 Matrix4::Perspective(float fov, float aspect, float znear, float zfar)
{
    float ymax = znear * tanf(fov * 3.14159265359f / 360.0f);
    float xmax = ymax * aspect;
    return Frustum(-xmax, xmax, -ymax, ymax, znear, zfar);
}

Matrix4 Matrix4::Ortho(float l, float r, float b, float t, float n, float f)
{
    if (l == r || t == b || n == f) {
        ASSERT(!"INVALID_CODE_PATH");
    }
    Matrix4 mat =  Matrix4(
       2.0f / (r - l), 0, 0, -((r + l) / (r - l)),
       0, 2.0f / (t - b), 0, -(t + b) / (t - b),
       0, 0, 1.0f / (f - n), -(n / (f - n)),
       0, 0, 0, 1);
    return Matrix4::Transposed(mat);
}

Matrix4 Matrix4::LookAt(const Vector3& position, const Vector3& target, const Vector3& up)
{
    Vector3 zaxis = (target - position).Normalized();
    Vector3 xaxis = up.Cross(zaxis).Normalized();
    Vector3 yaxis = zaxis.Cross(xaxis).Normalized();
    Matrix4 mat =  Matrix4(
        xaxis.x, xaxis.y, xaxis.z, -xaxis.Dot(position), 
        yaxis.x, yaxis.y, yaxis.z, -yaxis.Dot(position), 
        zaxis.x, zaxis.y, zaxis.z, -zaxis.Dot(position),
        0,       0,       0,       1);
    return Matrix4::Transposed(mat);
}

Matrix4 Matrix4::Translate(float x, float y, float z)
{
    return Matrix4(
        1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, 0,
        x, y, z, 1);
}

Matrix4 Matrix4::Translate(const Vector3& pos)
{
    return Translate(pos.x, pos.y, pos.z);
}

Matrix4 Matrix4::Scale(float x, float y, float z)
{
    return Matrix4(
        x, 0, 0, 0,
        0, y, 0, 0,
        0, 0, z, 0,
        0, 0, 0, 1);
}

Matrix4 Matrix4::Scale(const Vector3& scale)
{
    return Scale(scale.x, scale.y, scale.z);
}

Matrix4 Matrix4::RotateX(float angle)
{
    return Matrix4(
        1, 0, 0, 0,
        0, cosf(angle), sinf(angle), 0,
        0, -sinf(angle), cosf(angle), 0,
        0, 0, 0, 1);
}

Matrix4 Matrix4::RotateY(float angle)
{
    return Matrix4(
        cosf(angle), 0, -sinf(angle), 0,
        0, 1, 0, 0,
        sinf(angle), 0, cosf(angle), 0,
        0, 0, 0, 1);
}

Matrix4 Matrix4::RotateZ(float angle)
{
    return Matrix4(
        cosf(angle), sinf(angle), 0, 0,
        -sinf(angle), cosf(angle), 0, 0,
        0, 0, 1, 0,
        0, 0, 0, 1);
}

Matrix4 Matrix4::TransformFromBasis(const Vector3& o, const Vector3& r, const Vector3& u, const Vector3& f)
{   
    return Matrix4(
        r.x, r.y, r.z, 0,
        u.x, u.y, u.z, 0,
        f.x, f.y, f.z, 0,
        o.x, o.y, o.z, 1);
}

Matrix4 Matrix4::TransformFromEuler(float x, float y, float z)
{
    return Matrix4::RotateZ(z) * Matrix4::RotateX(x) * Matrix4::RotateY(y);
}

Matrix4 Matrix4::TransformFromEuler(const Vector3& rotation)
{
    return Matrix4::RotateZ(rotation.z) * Matrix4::RotateX(rotation.x) * Matrix4::RotateY(rotation.y);
}

Matrix4 Matrix4::Transposed(const Matrix4 &m)
{
	return {
		m.m11, m.m21, m.m31, m.m41,
		m.m12, m.m22, m.m32, m.m42,
		m.m13, m.m23, m.m33, m.m43,
		m.m14, m.m24, m.m34, m.m44
    };
}

#define M4_3X3MINOR(c0, c1, c2, r0, r1, r2) \
    (m.v[c0 * 4 + r0] * (m.v[c1 * 4 + r1] * m.v[c2 * 4 + r2] - m.v[c1 * 4 + r2] * m.v[c2 * 4 + r1]) - \
     m.v[c1 * 4 + r0] * (m.v[c0 * 4 + r1] * m.v[c2 * 4 + r2] - m.v[c0 * 4 + r2] * m.v[c2 * 4 + r1]) + \
     m.v[c2 * 4 + r0] * (m.v[c0 * 4 + r1] * m.v[c1 * 4 + r2] - m.v[c0 * 4 + r2] * m.v[c1 * 4 + r1]))

float Matrix4::Determinant(const Matrix4 &m)
{
	return  m.v[0] * M4_3X3MINOR(1, 2, 3, 1, 2, 3)
		  - m.v[4] * M4_3X3MINOR(0, 2, 3, 1, 2, 3)
		  + m.v[8] * M4_3X3MINOR(0, 1, 3, 1, 2, 3)
		  - m.v[12] * M4_3X3MINOR(0, 1, 2, 1, 2, 3);
}

Matrix4 Matrix4::Adjugate(const Matrix4 &m)
{
	// Cofactor(M[i, j]) = Minor(M[i, j]] * pow(-1, i + j)
	Matrix4 cofactor;

	cofactor.v[0] =  M4_3X3MINOR(1, 2, 3, 1, 2, 3);
	cofactor.v[1] = -M4_3X3MINOR(1, 2, 3, 0, 2, 3);
	cofactor.v[2] =  M4_3X3MINOR(1, 2, 3, 0, 1, 3);
	cofactor.v[3] = -M4_3X3MINOR(1, 2, 3, 0, 1, 2);

	cofactor.v[4] = -M4_3X3MINOR(0, 2, 3, 1, 2, 3);
	cofactor.v[5] =  M4_3X3MINOR(0, 2, 3, 0, 2, 3);
	cofactor.v[6] = -M4_3X3MINOR(0, 2, 3, 0, 1, 3);
	cofactor.v[7] =  M4_3X3MINOR(0, 2, 3, 0, 1, 2);

	cofactor.v[8] =   M4_3X3MINOR(0, 1, 3, 1, 2, 3);
	cofactor.v[9] =  -M4_3X3MINOR(0, 1, 3, 0, 2, 3);
	cofactor.v[10] =  M4_3X3MINOR(0, 1, 3, 0, 1, 3);
	cofactor.v[11] = -M4_3X3MINOR(0, 1, 3, 0, 1, 2);

	cofactor.v[12] = -M4_3X3MINOR(0, 1, 2, 1, 2, 3);
	cofactor.v[13] =  M4_3X3MINOR(0, 1, 2, 0, 2, 3);
	cofactor.v[14] = -M4_3X3MINOR(0, 1, 2, 0, 1, 3);
	cofactor.v[15] =  M4_3X3MINOR(0, 1, 2, 0, 1, 2);

	return Matrix4::Transposed(cofactor);
}

Matrix4 Matrix4::Inverse(const Matrix4 &m)
{
	float det = Matrix4::Determinant(m);
	if (det == 0.0f)
    { // Epsilon check would need to be REALLY small
        ASSERT(!"INVALID_CODE_PATH");
	}
	Matrix4 adj = Matrix4::Adjugate(m);
	return adj * (1.0f / det);
}