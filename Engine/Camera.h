#pragma once

#include "framework.h"

class Entity;

class EXPORT Camera
{
public:
    Entity* owner;
    float fov = 45.0f;
};
