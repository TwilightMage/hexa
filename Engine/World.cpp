#include "World.h"


#include "Game.h"
#include "ITickable.h"
#include "Mesh.h"

void World::setup_spawn(Entity* entity)
{
    entity->world_ = this;
    entities_.Add(Shared<Entity>(entity));
    entity->start();
    if (entity->get_mesh() != nullptr &&  entity->get_shader() != nullptr)
    {
        notify_renderable_updated(entity, nullptr);
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

void World::notify_renderable_updated(IRenderable* renderable, class Mesh* old_mesh)
{
    // TODO: Rework so we will have render_database.add, render_database.remove, render_database.transfer_mesh, render_database.transfer_shader
    Mesh* new_mesh = renderable->get_mesh();


    if (renderable->get_mesh() != old_mesh)
    {
        if (!old_mesh) // add
        {
            Game::get_instance()->render_database.add(renderable);
        }
        else if (!new_mesh) // remove
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
