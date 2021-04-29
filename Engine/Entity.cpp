#include "Entity.h"


#include "Game.h"
#include "Mesh.h"
#include "World.h"

World* Entity::get_world() const
{
    return world_;
}

void Entity::start()
{
    started_ = true;
}

void Entity::destroy()
{
    pending_kill_ = true;
}

bool Entity::is_started() const
{
    return started_;
}

void Entity::use_mesh(Mesh* new_mesh)
{
    if (mesh_)
    {
        mesh_->usage_count_--;
    }

    if (new_mesh)
    {
        new_mesh->usage_count_++;
    }

    const auto old_mesh = mesh_;
    mesh_ = Shared<Mesh>(new_mesh);

    if (started_ && !get_shader().expired())
    {
        world_->notify_renderable_updated(this, old_mesh);
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

    if (started_ && get_shader().expired())
    {
        world_->notify_renderable_updated(this, old_mesh);
    }
}

Weak<Mesh> Entity::get_mesh() const
{
    return mesh_;
}

Weak<Shader> Entity::get_shader() const
{
    return Game::get_instance()->get_basic_shader();
}
