#include "World.h"


#include "Game.h"
#include "ITickable.h"
#include "Mesh.h"
#include "Quaternion.h"
#include "Renderer.h"

void World::spawn_entity(const Weak<Entity>& entity, const Vector3& pos, const Quaternion& rot)
{
    if (auto entity_ptr = entity.lock())
    {
        entity_ptr->position = pos;
        entity_ptr->rotation = rot;
        spawn_entity(entity);
    }
}

void World::spawn_entity(const Weak<Entity>& entity, const Vector3& pos)
{
    if (auto entity_ptr = entity.lock())
    {
        entity_ptr->position = pos;
        spawn_entity(entity);
    }
}

void World::spawn_entity(const Weak<Entity>& entity, const Quaternion& rot)
{
    if (auto entity_ptr = entity.lock())
    {
        entity_ptr->rotation = rot;
        spawn_entity(entity);
    }
}

void World::spawn_entity(const Weak<Entity>& entity)
{
    if (auto entity_ptr = entity.lock())
    {
        entity_ptr->world_ = weak_from_this();
        entities_.Add(entity_ptr);
        entity_ptr->on_start();
        if (entity_ptr->get_mesh() && entity_ptr->get_shader())
        {
            notify_renderable_added(entity_ptr);
        }
    }
}

void World::start()
{
    on_start();

    for (auto entity : entities_)
    {
        notify_renderable_added(entity);
    }
}

void World::tick(float delta_time)
{
    List<uint> to_delete;
    
    on_tick();
    
    for (uint i = 0; i < entities_.length(); i++)
    {
        if (entities_[i]->pending_kill_)
        {
            if (entities_[i]->get_mesh() && entities_[i]->get_shader())
            {
                notify_renderable_deleted(entities_[i]);
            }

            entities_[i]->on_destroy();
            entities_[i]->pending_kill_ = false;
            to_delete.Add(i);
        }
        else if (auto tickable = cast<ITickable>(entities_[i]))
        {
            tickable->tick(delta_time);
        }
    }

    for (uint i = 0; i < to_delete.length(); i++)
    {
        entities_.RemoveAt(to_delete[to_delete.length() - 1 - i]);
    }
}

const List<Shared<Entity>>& World::get_entities() const
{
    return entities_;
}

void World::notify_renderable_added(const Weak<IRenderable>& renderable)
{
    Game::get_instance()->renderer_->register_object(renderable);
}

void World::notify_renderable_deleted(const Weak<IRenderable>& renderable)
{
    Game::get_instance()->renderer_->unregister_object(renderable);
}

void World::notify_renderable_mesh_updated(const Weak<IRenderable>& renderable, const Weak<Mesh>& old_mesh)
{
    if (auto renderable_ptr = renderable.lock())
    {
        const auto new_mesh_ptr = renderable_ptr->get_mesh();
        const auto old_mesh_ptr = old_mesh.lock();


        if (new_mesh_ptr != old_mesh_ptr)
        {
            if (!old_mesh_ptr) // add
            {
                Game::get_instance()->renderer_->register_object(renderable_ptr);
            }
            else if (!new_mesh_ptr) // remove
            {
                Game::get_instance()->renderer_->unregister_object(renderable_ptr);
            }
            else // change
            {
                Game::get_instance()->renderer_->change_object_mesh(renderable_ptr, old_mesh);
            }
        }
    }
}

void World::notify_renderable_shader_updated(const Weak<IRenderable>& renderable, const Weak<Shader>& old_shader)
{
    if (auto renderable_ptr = renderable.lock())
    {
        Game::get_instance()->renderer_->change_object_shader(renderable_ptr, old_shader);
    }
}

void World::on_start()
{
}

void World::on_tick()
{
}

void World::on_close()
{
}
