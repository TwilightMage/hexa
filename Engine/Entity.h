#pragma once

#include "BasicTypes.h"
#include "framework.h"
#include "IRenderable.h"
#include "Object.h"
#include "Quaternion.h"
#include "Slot.h"
#include "Texture.h"
#include "Vector3.h"

namespace reactphysics3d
{
    class RigidBody;
    class Collider;
}

EXTERN class EXPORT Entity : public Object, public IRenderable, public std::enable_shared_from_this<Entity>
{
    friend class World;

public:
    Entity();
    
    Weak<World> get_world() const;
    
    virtual void on_start();
    virtual void on_destroy();

    void destroy();

    bool is_started() const;
    
    void use_mesh(const Weak<class Mesh>& new_mesh);
    void clear_mesh();
    void use_shader(const Weak<Shader>& new_shader);
    void use_texture(const Weak<Texture>& new_texture);
    Shared<Mesh> get_mesh() const override;
    Shared<Shader> get_shader() const override;
    Shared<Texture> get_texture() const override;
    glm::mat4 get_matrix() const override;
    Vector3 get_position() const;
    void set_position(const Vector3& pos);
    Quaternion get_rotation() const;
    void set_rotation(const Quaternion& rot);
    Vector3 get_scale() const;
    void set_scale(const Vector3& scale);

    void use_sphere_collision(float radius, const Vector3& offset = Vector3::zero());
    void remove_collision();

protected:
    virtual bool is_rigid_body();
    
private:
    void cache_matrix();
    
    void start();

    Vector3 position_;
    Quaternion rotation_;
    Vector3 scale_ = Vector3::one();
    glm::mat4 cached_matrix_;
    
    Weak<World> world_;
    Shared<Mesh> mesh_;
    Shared<Shader> shader_;
    Slot<Texture> texture_;
    bool pending_kill_;
    bool started_;
    reactphysics3d::RigidBody* rigid_body_;
    reactphysics3d::Collider* collider_;
};
