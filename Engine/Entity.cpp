#include "Entity.h"

#include <reactphysics3d/body/RigidBody.h>
#include <reactphysics3d/engine/PhysicsCommon.h>


#include "Assert.h"
#include "Game.h"
#include "Mesh.h"
#include "GeometryEditor.h"
#include "Texture.h"
#include "World.h"
#include "Physics/Collision.h"

Entity::Entity()
    : Object(typeid(this).name() + String(" entity"))
    , shader_(Game::get_basic_shader())
    , texture_(Game::get_white_pixel(), false)
    , pending_kill_(false)
    , started_(false)
    , rigid_body_(nullptr)
    , collider_(nullptr)
    , is_matrix_dirty_(true)
{
}

Shared<World> Entity::get_world() const
{
    return world_.lock();
}

void Entity::on_start()
{
}

void Entity::on_destroy()
{
}

void Entity::destroy()
{
    pending_kill_ = true;
}

bool Entity::is_started() const
{
    return started_;
}

void Entity::use_mesh(const Weak<Mesh>& new_mesh)
{
    if (!world_.expired())
    {
        if (mesh_)
        {
            mesh_->usage_count_--;
        }

        if (const auto new_mesh_ptr = new_mesh.lock())
        {
            new_mesh_ptr->usage_count_++;
        }
    }

    const auto old= mesh_;
    mesh_ = new_mesh.lock();

    if (mesh_ != old)
    {
        if (mesh_)
        {
            texture_.activate();
        }
        else
        {
            texture_.deactivate();
        }
        
        if (started_)
        {
            if (auto world_ptr = world_.lock())
            {
                world_ptr->notify_renderable_mesh_updated(weak_from_this(), old);
            }
        }
    }
}

void Entity::clear_mesh()
{
    if (mesh_ && !world_.expired())
    {
        mesh_->usage_count_--;
    }

    const auto old_mesh = mesh_;
    mesh_ = nullptr;

    texture_.deactivate();

    if (auto world_ptr = world_.lock())
    {
        world_ptr->notify_renderable_mesh_updated(weak_from_this(), old_mesh);
    }
}

void Entity::use_shader(const Weak<Shader>& new_shader)
{
    const auto shader_to_set = new_shader.expired() ? Game::get_basic_shader() : new_shader.lock();
    if (shader_ != shader_to_set)
    {
        const auto old = shader_;
        shader_ = shader_to_set;
        
        if (auto world_ptr = world_.lock())
        {
            world_ptr->notify_renderable_shader_updated(weak_from_this(), old);
        }
    }
}

void Entity::use_texture(const Weak<Texture>& new_texture)
{
    const auto texture_to_set = new_texture.expired() ? Game::get_white_pixel() : new_texture.lock();
    if (*texture_ != texture_to_set)
    {
        texture_ = texture_to_set;
    }
}

Shared<Mesh> Entity::get_mesh() const
{
    return mesh_;
}

Shared<Shader> Entity::get_shader() const
{
    return shader_;
}

Shared<Texture> Entity::get_texture() const
{
    return *texture_;
}

void Entity::mark_matrix_dirty()
{
    is_matrix_dirty_ = true;
}

Matrix4x4 Entity::get_matrix() const
{
    return cached_matrix_;
}

Vector3 Entity::get_position() const
{
    if (rigid_body_)
    {
        return cast_object<Vector3>(rigid_body_->getTransform().getPosition());
    }
    
    return position_;
}

void Entity::set_position(const Vector3& pos)
{
    if (rigid_body_)
    {
        auto trans = rigid_body_->getTransform();
        trans.setPosition(reactphysics3d::Vector3(pos.x, pos.y, pos.z));
        rigid_body_->setTransform(trans);
    }
    else
    {
        position_ = pos;
    }

    is_matrix_dirty_ = true;
}

void Entity::translate(const Vector3& translation)
{
    set_position(get_position() + translation);
}

Quaternion Entity::get_rotation() const
{
    if (rigid_body_)
    {
        return cast_object<Quaternion>(rigid_body_->getTransform().getOrientation());
    }
    
    return rotation_;
}

void Entity::set_rotation(const Quaternion& rot)
{
    if (rigid_body_)
    {
        auto trans = rigid_body_->getTransform();
        trans.setOrientation(reactphysics3d::Quaternion(rot.x, rot.y, rot.z, rot.w));
        rigid_body_->setTransform(trans);
    }
    else
    {
        rotation_ = rot;
    }

    is_matrix_dirty_ = true;
}

Vector3 Entity::get_scale() const
{
    return scale_;
}

void Entity::set_scale(const Vector3& scale)
{
    scale_ = scale;

    is_matrix_dirty_ = true;
}

void Entity::use_collision(const Shared<Collision>& collision, const Vector3& offset)
{
    remove_collision();

    const auto collider_shape = collision->get_collider_shape();

    Assert(collider_shape != nullptr);
    
    collision_ = collision;
    collider_ = rigid_body_->addCollider(collider_shape, reactphysics3d::Transform(cast_object<reactphysics3d::Vector3>(offset), reactphysics3d::Quaternion::identity()));
    collider_->setCollisionCategoryBits(collision_mask_);
}

void Entity::remove_collision()
{
    if (collider_)
    {
        rigid_body_->removeCollider(collider_);
        collider_ = nullptr;
    }

    collision_ = nullptr;
}

void Entity::use_collision_mask(byte16 bits)
{
    collision_mask_ = bits;
    
    if (collider_)
    {
        collider_->setCollisionCategoryBits(bits);
    }
}

byte16 Entity::get_collision_mask() const
{
    return collision_mask_;
}

void Entity::set_gravity_enabled(bool state) const
{
    rigid_body_->enableGravity(state);
}

void Entity::make_body_static() const
{
    rigid_body_->setType(reactphysics3d::BodyType::STATIC);
}

void Entity::make_body_dynamic() const
{
    rigid_body_->setType(reactphysics3d::BodyType::DYNAMIC);
}

void Entity::make_body_kinematic() const
{
    rigid_body_->setType(reactphysics3d::BodyType::KINEMATIC);
}

void Entity::add_component(const Shared<EntityComponent>& component)
{
    Assert(component->get_owner() == nullptr);

    component->owner = weak_from_this();
    components_.Add(component);
}

void Entity::remove_component(const Shared<EntityComponent>& component)
{
    if (components_.Contains(component))
    {
        components_.Remove(component);
        component->on_destroy();
        component->owner = null_weak(Entity);
    }
}

void Entity::remove_all_components()
{
    for (auto& component : components_)
    {
        component->on_destroy();
        component->owner = null_weak(Entity);
    }
    components_.Clear();
}

bool Entity::is_rigid_body()
{
    return false;
}

void Entity::generate_components()
{
}

void Entity::modify_matrix_params(Vector3& position, Quaternion& rotation, Vector3& scale)
{
}

void Entity::cache_matrix()
{
    Vector3 position = get_position();
    Quaternion rotation = get_rotation();
    Vector3 scale = get_scale();
    modify_matrix_params(position, rotation, scale);
    cached_matrix_ = Matrix4x4().translate(position).rotate(rotation).scale(scale);
    is_matrix_dirty_ = false;
}

void Entity::start()
{
    started_ = true;
    
    if (mesh_)
    {
        texture_.activate();
    }
    on_start();
}
