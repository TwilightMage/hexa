#pragma once

#include "Collision.h"
#include "Engine/Vector3.h"

namespace reactphysics3d {
    class BoxShape;
}

EXTERN class EXPORT BoxCollision : public Collision
{
public:
    BoxCollision(const Vector3& extent);
    ~BoxCollision();

private:
    FORCEINLINE reactphysics3d::CollisionShape* get_collider_shape() const override;

    reactphysics3d::BoxShape* shape_;
};
