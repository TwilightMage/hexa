#pragma once
#include <glm/vec3.hpp>
#include <glm/ext/quaternion_float.hpp>

#include "Entity.h"

EXTERN class EXPORT Camera
{
public:
    class Entity* owner;
    float fov = 45.0f;
};
