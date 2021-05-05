#include "Entity.h"


#include "Game.h"
#include "Mesh.h"
#include "Texture.h"
#include "World.h"

Entity::Entity()
    : shader_(Game::get_basic_shader())
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
    started_ = true;
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
    if (mesh_)
    {
        mesh_->usage_count_--;
    }

    if (const auto new_mesh_ptr = new_mesh.lock())
    {
        new_mesh_ptr->usage_count_++;
    }

    const auto old= mesh_;
    mesh_ = new_mesh.lock();

    if (mesh_ != old)
    {
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
    if (mesh_)
    {
        mesh_->usage_count_--;
    }

    const auto old_mesh = mesh_;
    mesh_ = nullptr;

    if (started_)
    {
        if (auto world_ptr = world_.lock())
        {
            world_ptr->notify_renderable_mesh_updated(weak_from_this(), old_mesh);
        }
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
        if (started_)
        {
            if (auto world_ptr = world_.lock())
            {
                world_ptr->notify_renderable_shader_updated(weak_from_this(), old);
            }
        }
    }
}

void Entity::clear_shader()
{
    const auto old = shader_;
    shader_ = Game::get_basic_shader();

    if (shader_ != old)
    {
        if (started_)
        {
            if (auto world_ptr = world_.lock())
            {
                world_ptr->notify_renderable_shader_updated(weak_from_this(), old);
            }
        }
    }
}

void Entity::use_texture(const Weak<Texture>& new_texture)
{
    if (const auto texture_ptr = new_texture.lock())
    {
        texture_->usage_count_--;
        texture_->usage_count_changed();
        
        texture_ = texture_ptr;

        texture_->usage_count_++;
        texture_->usage_count_changed();
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
        texture_->usage_count_--;
        texture_->usage_count_changed();
        
        texture_ = Game::get_white_pixel();

        texture_->usage_count_++;
        texture_->usage_count_changed();
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
    return position;
}

Quaternion Entity::get_rotation() const
{
    return rotation;
}

Vector3 Entity::get_scale() const
{
    return scale;
}
