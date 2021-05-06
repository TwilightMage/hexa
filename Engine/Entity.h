#pragma once

#include "BasicTypes.h"
#include "framework.h"
#include "IRenderable.h"
#include "Object.h"
#include "String.h"

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
    void clear_shader();
    void use_texture(const Weak<Texture>& new_texture);
    void clear_texture();
    Shared<Mesh> get_mesh() const override;
    Shared<Shader> get_shader() const override;
    Shared<Texture> get_texture() const override;
    Vector3 get_position() const override;
    void set_position(const Vector3& pos);
    Quaternion get_rotation() const override;
    void set_rotation(const Quaternion& rot);
    Vector3 get_scale() const override;

    void use_sphere_collision(float radius, const Vector3& offset = Vector3::zero());
    void remove_collision();

    Vector3 scale_ = Vector3::one();
protected:
    virtual bool is_rigid_body();
    
private:
    void start();
    bool should_use_texture() const;

    Vector3 position_;
    Quaternion rotation_;
    
    
    Weak<World> world_;
    Shared<Mesh> mesh_;
    Shared<Shader> shader_;
    Shared<Texture> texture_;
    bool pending_kill_;
    bool started_;
    reactphysics3d::RigidBody* rigid_body_;
    reactphysics3d::Collider* collider_;
};
