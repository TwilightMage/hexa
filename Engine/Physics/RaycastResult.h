#pragma once

#include "Engine/Pointers.h"
#include "Engine/Vector3.h"

class Entity;

struct RaycastResult
{
    Vector3 location;
    Vector3 normal;
    int triangle_index;
    Shared<Entity> entity;
};
