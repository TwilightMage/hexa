#include "World.h"


#include "Game.h"
#include "ITickable.h"
#include "Mesh.h"

void World::setup_spawn(Entity* entity)
{
    entity->world_ = this;
    entities_.Add(Shared<Entity>(entity));
    entity->start();
    if (!entity->get_mesh().expired() &&  !entity->get_shader().expired())
    {
        notify_renderable_added(entity);
    }
}

void World::start()
{
    on_start();

    for (auto entity : entities_)
    {
        Game::get_instance()->render_database.add(entity.get());
    }
}

void World::tick(float delta_time)
{
    on_tick();
    for (uint i = 0; i < entities_.Length(); i++)
    {
        if (entities_[i]->pending_kill_)
        {
            entities_[i]->pending_kill_ = false;
            entities_.RemoveAt(i);
        }
        else if (auto tickable = Cast<ITickable>(entities_[i]))
        {
            tickable->tick(delta_time);
        }
    }
}

const List<Shared<Entity>>& World::get_entities() const
{
    return entities_;
}

void World::notify_renderable_added(IRenderable* renderable)
{
    notify_renderable_updated(renderable, Shared<Mesh>(nullptr));
}

void World::notify_renderable_updated(IRenderable* renderable, Weak<Mesh> old_mesh)
{
    // TODO: Rework so we will have render_database.add, render_database.remove, render_database.transfer_mesh, render_database.transfer_shader
    const auto new_mesh_ptr = renderable->get_mesh().lock();
    const auto old_mesh_ptr = old_mesh.lock();


    if (new_mesh_ptr != old_mesh_ptr)
    {
        if (!old_mesh_ptr) // add
        {
            Game::get_instance()->render_database.add(renderable);
        }
        else if (!new_mesh_ptr) // remove
        {
            //Game::get_instance()->render_database.remove(renderable);
        }
        else
        {
            //Game::get_instance()->render_database.transfer_mesh(renderable, old_mesh);
        }
    }
}

void World::on_start()
{
}

void World::on_tick()
{
}
