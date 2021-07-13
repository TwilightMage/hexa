﻿#include "World.h"

#include <reactphysics3d/reactphysics3d.h>
#include <soloud/soloud_wav.h>

#include "Audio.h"
#include "AudioChannel.h"
#include "Game.h"
#include "Quaternion.h"
#include "Material.h"
#include "performance.h"
#include "Settings.h"
#include "Physics/ConcaveMeshCollision.h"
#include "Physics/ConvexMeshCollision.h"
#include "Physics/RaycastCallback.h"

bool World::spawn_entity(const Shared<Entity>& entity, const Vector3& pos, const Quaternion& rot)
{
    if (entities_.contains(entity)) return false;
    
    entity->position_ = pos;
    entity->rotation_ = rot;
    spawn_entity_internal(entity);
    return true;
}

bool World::spawn_entity(const Shared<Entity>& entity, const Vector3& pos)
{
    if (entities_.contains(entity)) return false;
    entity->position_ = pos;
    spawn_entity_internal(entity);
    return true;
}

bool World::spawn_entity(const Shared<Entity>& entity, const Quaternion& rot)
{
    if (entities_.contains(entity)) return false;
    
    entity->rotation_ = rot;
    spawn_entity_internal(entity);
    return true;
}

bool World::spawn_entity(const Shared<Entity>& entity)
{
    if (entities_.contains(entity)) return false;
    
    spawn_entity_internal(entity);
    return true;
}

SoundHandle World::play_sound(const Shared<Audio>& audio, const Shared<AudioChannel>& channel)
{
    if (audio == nullptr) return SoundHandle();
    return SoundHandle((channel ? channel : Game::get_general_channel())->bus_->play(*(audio->sample_), audio->get_default_volume()), Game::instance_->soloud_);
}

SoundHandle World::play_sound_3d(const Shared<Audio>& audio, const Vector3& location, const Shared<AudioChannel>& channel)
{
    if (audio == nullptr) return SoundHandle();
    return SoundHandle((channel ? channel : Game::get_general_channel())->bus_->play3d(*(audio->sample_), location.x, location.y, location.z, 0, 0, 0, audio->get_default_volume()), Game::instance_->soloud_);
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

List<RaycastResult> World::raycast_all(const Vector3& from, const Vector3& to, bool sort_by_distance) const
{
    return raycast_all(from, to, CollisionMaskBits::ALL, sort_by_distance);
}

List<RaycastResult> World::raycast_all(const Vector3& from, const Vector3& to, byte16 collision_mask, bool sort_by_distance) const
{
    RaycastCallback callback;
    physics_world_->raycast(reactphysics3d::Ray(cast_object<reactphysics3d::Vector3>(from), cast_object<reactphysics3d::Vector3>(to)), &callback, collision_mask);

    if (sort_by_distance)
    {
        callback.results.sort([&](const RaycastResult& a, const RaycastResult& b)->bool
        {
            return Vector3::distance(from, a.location) > Vector3::distance(from, b.location);
        });
    }
    
    return callback.results;
}

void World::start()
{
    physics_world_ = Game::instance_->physics_->createPhysicsWorld();
    reactphysics3d::Vector3 gravity(0.0f, 0.0f, -9.81f);
    physics_world_->setGravity(gravity);
    
    on_start();
}

void World::tick(float delta_time)
{
    delta_time *= time_scale_;

    time_ += delta_time;

    // fixed tick for physics
    if (delta_time != 0.0f)
    {
        physics_tick_accum_ += delta_time;
        const auto interval = Game::get_settings()->get_physics_tick_interval();
        /*while (physics_tick_accum_ > interval)
        {
            //physics_world_->update(interval);
            //physics_tick_accum_ -= interval;
            
            physics_world_->update(physics_tick_accum_);
            physics_tick_accum_ = 0;
        }*/
    }

    // tick timers
    List<TimerHandle> expired_timers;
    for (auto& timer_entry : timer_entries_)
    {
        timer_entry.value.time -= delta_time;
        if (timer_entry.value.time <= 0)
        {
            timer_entry.value.func();
            expired_timers.add(timer_entry.key);
        }
    }

    // cleanup timers
    for (const auto& expired_timer : expired_timers)
    {
        timer_entries_.remove(expired_timer);
    }

    // tick in child
    on_tick(delta_time);

    // tick entities
    Set<Shared<Entity>> to_delete;

    auto camera_info = Game::get_camera_info();

    if (time_scale_ != 0.0f)
    {
        for (auto& entity : entities_)
        {
            if (entity->pending_kill_)
            {
                do_destroy(entity);

                entity->pending_kill_ = false;
            
                to_delete.add(entity);
            }
            else
            {
                if (entity->tick_enabled)
                {
                    entity->on_tick(delta_time);
                    for (auto component : entity->components_)
                    {
                        component->on_tick(delta_time);
                    }
                }
                if (entity->is_physically_dynamic_ || entity->is_matrix_dirty_)
                {
                    entity->cache_matrix();
                }

                entity->distance_to_camera_ = Vector3::distance(camera_info.position, entity->position_);
            }
        }
    }

    // cleanup entities
    for (auto& entity : to_delete)
    {
        entities_.remove(entity);
    }
    to_delete.clear();
}

const Set<Shared<Entity>>& World::get_entities() const
{
    return entities_;
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

TimerHandle World::delay(float time, std::function<void()> func)
{
    const TimerHandle handle = { TimerHandle::id_generator++ };
    timer_entries_.insert(handle, { time, func });
    return handle;
}

void World::on_start()
{
}

void World::on_tick(float delta_time)
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

    ConcaveMeshCollision::data_blocks_.clear();
    ConvexMeshCollision::data_blocks_.clear();
    
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
    entities_.add(entity);
    entity->start();
}

void World::do_destroy(const Shared<Entity>& entity)
{
    entity->on_destroy();

    entity->material_instance_->destroy();

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
