#pragma once

#include <math/vector4.h>

struct DirectionalLight
{
    Vector4 ambient;
    Vector4 diffuse;
    Vector4 specular;
    Vector3 direction;
    float pad;
};

struct PointLight
{
    Vector4 ambient;
    Vector4 diffuse;
    Vector4 specular;
    Vector3 position;
    float range;
    Vector3 att;
    float pad;
};