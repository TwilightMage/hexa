#pragma once

#include "CollisionMaskBits.h"
#include "EntityComponent.h"
#include "Matrix4x4.h"
#include "Name.h"
#include "Quaternion.h"
#include "Vector2.h"

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

    uint get_material_count() const;
    void set_material(const Shared<Material>& material, uint material_slot);
    Shared<Material> get_material(uint material_slot) const;

    void set_material_parameter(Quaternion value, uint material_slot, uint parameter_index);

    FORCEINLINE PhysicalBodyType get_body_type() const { return body_type_; }
    void set_body_type(PhysicalBodyType body_type);

    FORCEINLINE bool is_visible() const { return is_visible_; }
    void set_visibility(bool state);

private:
    void setup_new_mesh(const Shared<StaticMesh>& mesh);
    
    Shared<StaticMesh> mesh_;
    reactphysics3d::RigidBody* rigid_body_ = nullptr;
    reactphysics3d::Collider* collider_ = nullptr;
    Shared<Collision> collision_;
    byte16 collision_mask_ = CollisionMaskBits::NONE;
    PhysicalBodyType body_type_ = PhysicalBodyType::Dynamic;
    bool is_visible_ = true;

    Ogre::Entity* ogre_entity_ = nullptr;
};
