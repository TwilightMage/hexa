#pragma once

#include "Entity.h"

EXTERN class EXPORT Camera
{
public:
    class Entity* owner;
    float fov = 45.0f;
};
