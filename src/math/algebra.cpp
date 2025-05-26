#include "algebra.h"

float lerp(float a, float b, float t)
{
    return a*(1-t) + b*t;
}

float Radians(float degree)
{
    return degree * (ANT_PI / 180.0f);
}