#include "Entity.h"

#include <reactphysics3d/body/RigidBody.h>
#include <reactphysics3d/engine/PhysicsCommon.h>



#include "Game.h"
#include "Mesh.h"
#include "Texture.h"
#include "World.h"

Entity::Entity()
    : Object(typeid(this).name() + String(" entity"))
    , shader_(Game::get_basic_shader())
    , texture_(Game::get_white_pixel())
    , pending_kill_(false)
    , started_(false)
{
}

Weak<World> Entity::get_world() const
{
    return world_;
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
        if (!old)
        {
            if (should_use_texture())
            {
                texture_->usage_count_increase();
            }
        }
        else
        {
            if (should_use_texture())
            {
                texture_->usage_count_decrease();
            }
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

    if (should_use_texture())
    {
        texture_->usage_count_decrease();
    }

    if (auto world_ptr = world_.lock())
    {
        world_ptr->notify_renderable_mesh_updated(weak_from_this(), old_mesh);
    }
}

void Entity::use_shader(const Weak<Shader>& new_shader)
{
    const auto old = shader_;
    if (const auto shader_ptr = new_shader.lock())
    {
        shader_ = shader_ptr;
    }
    else
    {
        shader_ = Game::get_basic_shader();
    }

    if (shader_ != old)
    {
        if (auto world_ptr = world_.lock())
        {
            world_ptr->notify_renderable_shader_updated(weak_from_this(), old);
        }
    }
}

void Entity::clear_shader()
{
    const auto old = shader_;
    shader_ = Game::get_basic_shader();

    if (shader_ != old)
    {
        if (auto world_ptr = world_.lock())
        {
            world_ptr->notify_renderable_shader_updated(weak_from_this(), old);
        }
    }
}

void Entity::use_texture(const Weak<Texture>& new_texture)
{
    if (const auto texture_ptr = new_texture.lock())
    {
        if (should_use_texture())
        {
            texture_->usage_count_decrease();
        }
        
        texture_ = texture_ptr;

        if (should_use_texture())
        {
            texture_->usage_count_increase();
        }
    }
    else
    {
        clear_texture();
    }
}

void Entity::clear_texture()
{
    if (texture_ != Game::get_white_pixel())
    {
        if (should_use_texture())
        {
            texture_->usage_count_decrease();
        }
        
        texture_ = Game::get_white_pixel();

        if (should_use_texture())
        {
            texture_->usage_count_increase();
        }
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
    return texture_;
}

Vector3 Entity::get_position() const
{
    if (rigid_body_)
    {
        const auto pos = rigid_body_->getTransform().getPosition();
        return Vector3(pos.x, pos.y, pos.z);
    }
    else
    {
        return position_;
    }
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
}

Quaternion Entity::get_rotation() const
{
    if (rigid_body_)
    {
        const auto rot = rigid_body_->getTransform().getOrientation();
        return Quaternion(rot.x, rot.y, rot.z, rot.w);
    }
    else
    {
        return rotation_;
    }
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
}

Vector3 Entity::get_scale() const
{
    return scale_;
}

void Entity::use_sphere_collision(float radius, const Vector3& offset)
{
    remove_collision();

    collider_ = rigid_body_->addCollider(Game::instance_->physics_->createSphereShape(radius), reactphysics3d::Transform(reactphysics3d::Vector3(offset.x, offset.y, offset.z), reactphysics3d::Quaternion::identity()));
}

void Entity::remove_collision()
{
    if (collider_)
    {
        rigid_body_->removeCollider(collider_);
        collider_ = nullptr;
    }
}

bool Entity::is_rigid_body()
{
    return false;
}

void Entity::start()
{
    started_ = true;
    on_start();
}

bool Entity::should_use_texture() const
{
    return !world_.expired() && get_mesh();
}
