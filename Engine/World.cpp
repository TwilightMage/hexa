#include "World.h"

#include <reactphysics3d/reactphysics3d.h>

#include "Game.h"
#include "ITickable.h"
#include "Mesh.h"
#include "Quaternion.h"
#include "Renderer.h"
#include "Settings.h"
#include "Physics/RaycastCallback.h"

void World::spawn_entity(const Shared<Entity>& entity, const Vector3& pos, const Quaternion& rot)
{
    if (ensure_child_not_exist(entity)) return;
    
    entity->position_ = pos;
    entity->rotation_ = rot;
    spawn_entity_internal(entity);
}

void World::spawn_entity(const Shared<Entity>& entity, const Vector3& pos)
{
    if (ensure_child_not_exist(entity)) return;
    
    entity->position_ = pos;
    spawn_entity_internal(entity);
}

void World::spawn_entity(const Shared<Entity>& entity, const Quaternion& rot)
{
    if (ensure_child_not_exist(entity)) return;
    
    entity->rotation_ = rot;
    spawn_entity_internal(entity);
}

void World::spawn_entity(const Shared<Entity>& entity)
{
    if (ensure_child_not_exist(entity)) return;
    
    spawn_entity_internal(entity);
}

Shared<const RaycastResult> World::raycast(const Vector3& from, const Vector3& to) const
{
    return raycast(from, to, CollisionMaskBits::ALL);
}

Shared<const RaycastResult> World::raycast(const Vector3& from, const Vector3& to, byte16 collision_mask) const
{
    RaycastCallback callback;
    physics_world_->raycast(reactphysics3d::Ray(cast_object<reactphysics3d::Vector3>(from), cast_object<reactphysics3d::Vector3>(to)), &callback, collision_mask);
    callback.results.sort([&](const RaycastResult& a, const RaycastResult& b)->bool
    {
        return Vector3::distance(from, a.location) > Vector3::distance(from, b.location);
    });
    return callback.results.length() > 0 ? MakeShared<RaycastResult>(callback.results.first()) : nullptr;
}

void World::start()
{
    physics_world_ = Game::instance_->physics_->createPhysicsWorld();
    reactphysics3d::Vector3 gravity(0.0f, 0.0f, -9.81f);
    physics_world_->setGravity(gravity);
    
    on_start();

    for (auto entity : entities_)
    {
        notify_renderable_added(entity);
    }
}

void World::tick(float delta_time)
{
    delta_time *= time_scale_;

    if (delta_time != 0.0f)
    {
        physics_tick_accum_ += delta_time;
        const auto interval = Game::get_settings()->get_physics_tick_interval();
        while (physics_tick_accum_ > interval)
        {
            physics_tick_accum_ -= interval;
            physics_world_->update(interval);
        }
    }

    List<uint> to_delete;
    
    on_tick();
    
    for (uint i = 0; i < entities_.length(); i++)
    {
        if (auto& entity = entities_[i]; entity->pending_kill_)
        {
            do_destroy(entity);

            entity->pending_kill_ = false;
            
            to_delete.Add(i);
        }
        else if (time_scale_ != 0.0f)
        {            
            if (auto tickable = cast<ITickable>(entity))
            {
                tickable->tick(delta_time);
                for (auto component : entity->components_)
                {
                    component->on_tick(delta_time);
                }
            }

            if (entity->rigid_body_ && !entity->rigid_body_->isSleeping() || entity->is_matrix_dirty_)
            {
                entity->cache_matrix();
            }
        }
    }

    for (uint i = 0; i < to_delete.length(); i++)
    {
        entities_.RemoveAt(to_delete[to_delete.length() - 1 - i]);
    }
    to_delete.Clear();
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

float World::get_time_scale() const
{
    return time_scale_;
}

void World::set_time_scale(float val)
{
    time_scale_ = val;
}

Vector3 World::get_gravity() const
{
    const auto gravity = physics_world_->getGravity();
    return Vector3(gravity.x, gravity.y, gravity.z);
}

void World::set_gravity(const Vector3& val) const
{
    reactphysics3d::Vector3 gravity(val.x, val.y, val.z);
    physics_world_->setGravity(gravity);
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

void World::close()
{
    on_close();

    for (auto& entity : entities_)
    {
        do_destroy(entity);
    }
    
    Game::instance_->physics_->destroyPhysicsWorld(physics_world_);
    physics_world_ = nullptr;
}

void World::spawn_entity_internal(const Shared<Entity>& entity)
{
    entity->generate_components();
    entity->world_ = weak_from_this();
    if (entity->is_rigid_body())
    {
        entity->rigid_body_ = physics_world_->createRigidBody(
            reactphysics3d::Transform(
                reactphysics3d::Vector3(entity->position_.x, entity->position_.y, entity->position_.z),
                reactphysics3d::Quaternion(entity->rotation_.x, entity->rotation_.y, entity->rotation_.z, entity->rotation_.w)
            )
        );
        entity->rigid_body_->setUserData(entity.get());
    }
        
    if (const auto mesh = entity->get_mesh())
    {
        mesh->usage_count_++;
    }
        
    entities_.Add(entity);
    entity->start();

    if (entity->get_mesh() && entity->get_shader())
    {
        notify_renderable_added(entity);
    }
}

bool World::ensure_child_not_exist(const Shared<Entity>& entity)
{
    for (auto& existing_entity : entities_)
    {
        if (existing_entity == entity) return true;
    }

    return false;
}

void World::do_destroy(const Shared<Entity>& entity)
{
    entity->on_destroy();

    if (const auto mesh = entity->get_mesh())
    {
        mesh->usage_count_--;
    }
            
    if (entity->get_mesh() && entity->get_shader())
    {
        notify_renderable_deleted(entity);
    }

    if (entity->rigid_body_)
    {
        if (entity->collider_)
        {
            entity->rigid_body_->removeCollider(entity->collider_);
            entity->collision_ = nullptr;
        }

        physics_world_->destroyRigidBody(entity->rigid_body_);
        entity->rigid_body_ = nullptr;
    }

    entity->on_destroyed(entity);
}
