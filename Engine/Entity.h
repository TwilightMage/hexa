#pragma once

#include "BasicTypes.h"
#include "CollisionMaskBits.h"
#include "Delegate.h"
#include "EntityComponent.h"
#include "framework.h"
#include "IRenderable.h"
#include "Object.h"
#include "Quaternion.h"
#include "Material3DInstance.h"
#include "Vector3.h"

class Collision;
class Material3DInstance;

namespace reactphysics3d
{
    class RigidBody;
    class Collider;
}

class EXPORT Entity : public Object, public IRenderable<Material3D, Material3DInstance>, public std::enable_shared_from_this<Entity>
{
    friend class World;

public:
    Entity();
    
    Shared<World> get_world() const;
    
    virtual void on_start();
    virtual void on_destroy();

    void destroy();

    bool is_started() const;
    
    void set_mesh(const Shared<class Mesh>& new_mesh);
    void clear_mesh();

    void mark_matrix_dirty();
    
    FORCEINLINE Vector3 get_position() const;
    void set_position(const Vector3& pos);
    FORCEINLINE void translate(const Vector3& translation);
    
    FORCEINLINE Quaternion get_rotation() const;
    void set_rotation(const Quaternion& rot);
    
    FORCEINLINE Vector3 get_scale() const;
    void set_scale(const Vector3& scale);

    void set_collision(const Shared<Collision>& collision, const Vector3& offset = Vector3::zero());
    void remove_collision();

    void set_collision_mask(byte16 bits);
    byte16 get_collision_mask() const;

    void set_gravity_enabled(bool state) const;

    void make_body_static() const;
    void make_body_dynamic() const;
    void make_body_kinematic() const;

    void set_material(const Shared<Material3D>& material) override;
    FORCEINLINE Shared<Material3D> get_material() const override { return material_; }
    FORCEINLINE Shared<Material3DInstance> get_material_instance() const override;

    void add_component(const Shared<EntityComponent>& component);
    void remove_component(const Shared<EntityComponent>& component);
    void remove_all_components();
    template<class Class>
    Shared<Class> find_component()
    {
        for (auto& component : components_)
        {
            if (auto casted = cast<Class>(component))
            {
                return casted;
            }
        }

        return nullptr;
    }

    FORCEINLINE bool is_visible() const { return material_instance_->is_visible(); }
    FORCEINLINE void set_visibility(bool visibility) const { material_instance_->set_visible(visibility); }

    Delegate<const Shared<Entity>&> on_destroyed;
    
protected:
    virtual bool is_rigid_body() const { return false; }

    virtual void generate_components();

    virtual void modify_matrix_params(Vector3& position, Quaternion& rotation, Vector3& scale);
    
private:
    void cache_matrix();
    void material_changed();
    
    void start();

    Vector3 position_;
    Quaternion rotation_;
    Vector3 scale_ = Vector3::one();
    bool is_matrix_dirty_;
    
    Weak<World> world_;
    Shared<Material3DInstance> material_instance_;
    Shared<Material3D> material_;
    Shared<MaterialParameter<Matrix4x4>> model_parameter_;
    Shared<Mesh> mesh_;
    bool pending_kill_;
    bool started_;
    reactphysics3d::RigidBody* rigid_body_;
    reactphysics3d::Collider* collider_;
    Shared<Collision> collision_;
    byte16 collision_mask_ = CollisionMaskBits::NONE;
    List<Shared<EntityComponent>> components_;
};
