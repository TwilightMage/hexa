#pragma once

#include "Engine/framework.h"

namespace reactphysics3d {
    class CollisionShape;
    class PhysicsCommon;
    class Collider;
}

class EXPORT Collision
{
public:
    virtual reactphysics3d::CollisionShape* get_collider_shape() const = 0;

protected:
    FORCEINLINE static Shared<reactphysics3d::PhysicsCommon> get_physics();
};
