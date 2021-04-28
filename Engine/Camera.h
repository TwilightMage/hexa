#pragma once
#include <glm/vec3.hpp>
#include <glm/ext/quaternion_float.hpp>

#include "WorldObject.h"

class Camera
{
public:
    class WorldObject* owner;
    float fov = 45.0f;
};
