#pragma once

#include <OGRE/Main/OgrePrerequisites.h>

#include "CollisionMaskBits.h"
#include "EntityComponent.h"
#include "Name.h"

class Material;

namespace Ogre {
    class Entity;
}

class Collision;

namespace reactphysics3d
{
    class Collider;
    class RigidBody;
}

class StaticMesh;

class EXPORT MeshComponent : public EntityComponent
{
public:
    explicit MeshComponent();
    
    void on_start() override;
    void on_destroy() override;

    FORCEINLINE Shared<StaticMesh> get_mesh() const { return mesh_; }
    void set_mesh(const Shared<StaticMesh>& mesh);

    void set_material(const Shared<Material>& material, uint index);

private:
    Shared<StaticMesh> mesh_;
    reactphysics3d::RigidBody* rigid_body_ = nullptr;
    reactphysics3d::Collider* collider_ = nullptr;
    Shared<Collision> collision_;
    byte16 collision_mask_ = CollisionMaskBits::NONE;

    Ogre::Entity* ogre_entity_ = nullptr;
};
