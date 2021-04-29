#include "World.h"


#include "Game.h"
#include "ITickable.h"
#include "Mesh.h"

void World::setup_spawn(const Weak<Entity>& entity)
{
    if (auto entity_ptr = entity.lock())
    {
        entity_ptr->world_ = this;
        entities_.Add(entity_ptr);
        entity_ptr->start();
        if (!entity_ptr->get_mesh().expired() && !entity_ptr->get_shader().expired())
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
        Game::get_instance()->render_database.add(entity);
    }
}

void World::tick(float delta_time)
{
    List<uint> to_delete;
    
    on_tick();
    
    for (uint i = 0; i < entities_.Length(); i++)
    {
        if (entities_[i]->pending_kill_)
        {
            if (!entities_[i]->get_mesh().expired() && !entities_[i]->get_shader().expired())
            {
                notify_renderable_deleted(entities_[i]);
            }

            entities_[i]->on_destroy();
            entities_[i]->pending_kill_ = false;
            to_delete.Add(i);
        }
        else if (auto tickable = Cast<ITickable>(entities_[i]))
        {
            tickable->tick(delta_time);
        }
    }

    for (uint i = 0; i < to_delete.Length(); i++)
    {
        entities_.RemoveAt(to_delete[to_delete.Length() - 1 - i]);
    }
}

const List<Shared<Entity>>& World::get_entities() const
{
    return entities_;
}

void World::notify_renderable_added(const Weak<IRenderable>& renderable)
{
    Game::get_instance()->render_database.add(renderable);
}

void World::notify_renderable_deleted(const Weak<IRenderable>& renderable)
{
    Game::get_instance()->render_database.remove(renderable);
}

void World::notify_renderable_updated(const Weak<IRenderable>& renderable, const Weak<Mesh>& old_mesh)
{
    if (auto renderable_ptr = renderable.lock())
    {
        // TODO: Rework so we will have render_database.add, render_database.remove, render_database.transfer_mesh, render_database.transfer_shader
        const auto new_mesh_ptr = renderable_ptr->get_mesh().lock();
        const auto old_mesh_ptr = old_mesh.lock();


        if (new_mesh_ptr != old_mesh_ptr)
        {
            if (!old_mesh_ptr) // add
                {
                Game::get_instance()->render_database.add(renderable_ptr);
                }
            else if (!new_mesh_ptr) // remove
                {
                    Game::get_instance()->render_database.remove(renderable_ptr);
                }
            else
            {
                //Game::get_instance()->render_database.transfer_mesh(renderable_ptr, old_mesh);
            }
        }
    }
}

void World::on_start()
{
}

void World::on_tick()
{
}
