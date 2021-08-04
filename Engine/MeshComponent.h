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

enum class PhysicalBodyType
{
    Static,
    Kinematic,
    Dynamic
};

class EXPORT MeshComponent : public EntityComponent
{
public:
    explicit MeshComponent();
    
    void on_start() override;
    void on_destroy() override;

    FORCEINLINE Shared<StaticMesh> get_mesh() const { return mesh_; }
    void set_mesh(const Shared<StaticMesh>& mesh);

    void set_material(const Shared<Material>& material, uint index);

    FORCEINLINE PhysicalBodyType get_body_type() const { return body_type_; }
    void set_body_type(PhysicalBodyType body_type);

private:
    void setup_new_mesh(const Shared<StaticMesh>& mesh);
    
    Shared<StaticMesh> mesh_;
    reactphysics3d::RigidBody* rigid_body_ = nullptr;
    reactphysics3d::Collider* collider_ = nullptr;
    Shared<Collision> collision_;
    byte16 collision_mask_ = CollisionMaskBits::NONE;
    PhysicalBodyType body_type_ = PhysicalBodyType::Dynamic;

    Ogre::Entity* ogre_entity_ = nullptr;
};
