#include "World.h"

#include <OGRE/Main/OgreRoot.h>
#include <OGRE/Main/OgreSceneManager.h>
#include <reactphysics3d/reactphysics3d.h>
#include <soloud/soloud_wav.h>

#include "Audio.h"
#include "AudioChannel.h"
#include "CollisionMaskBits.h"
#include "Game.h"
#include "OgreApp.h"
#include "Quaternion.h"
#include "Settings.h"
#include "Texture.h"
#include "HexaGame/Entities/ItemDrop.h"
#include "Physics/RaycastCallback.h"
#include "OGRE/Main/OgreHardwarePixelBuffer.h"

bool World::spawn_entity(const Shared<Entity>& entity, const Transform& transform)
{
    if (entities_.contains(entity)) return false;
    
    entity->transform_ = transform;
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
    return SoundHandle((channel ? channel : Game::get_general_channel())->bus_->play(*(audio->sample_), audio->get_default_volume()));
}

SoundHandle World::play_sound_3d(const Shared<Audio>& audio, const Vector3& location, const Shared<AudioChannel>& channel)
{
    if (audio == nullptr) return SoundHandle();
    return SoundHandle((channel ? channel : Game::get_general_channel())->bus_->play3d(*(audio->sample_), location.x, location.y, location.z, 0, 0, 0, audio->get_default_volume()));
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

void World::init()
{
    physics_world_ = Game::instance_->physics_->createPhysicsWorld();
    reactphysics3d::Vector3 gravity(0.0f, 0.0f, -9.81f);
    physics_world_->setGravity(gravity);

    manager_ = Game::instance_->ogre_app_->getRoot()->createSceneManager();

    world_root_ = manager_->getRootSceneNode()->createChildSceneNode();
    //world_root_->scale(Ogre::Vector3(-1, -1, -1));
    //world_root_->rotate(Ogre::Vector3(1, 0, 0), Ogre::Degree(90), Ogre::Node::TS_WORLD);
    //world_root_->rotate(Ogre::Vector3(0, 0, 1), Ogre::Degree(-90), Ogre::Node::TS_WORLD);

    manager_->setShadowTechnique(Ogre::SHADOWTYPE_TEXTURE_MODULATIVE_INTEGRATED);
    manager_->setShadowTextureSelfShadow(true);
    manager_->setShadowTexturePixelFormat(Ogre::PF_FLOAT32_R);
    manager_->setShadowTextureCount(1);
    manager_->setShadowTextureSize(8192);
    manager_->setShadowFarDistance(5000);

    manager_->setSkyBox(true, "Hexa/Skybox", 300, true);

    set_ambient_light(Color::white(), 0.5f);

    directional_light_ = manager_->createLight();
    directional_light_->setType(Ogre::Light::LT_DIRECTIONAL);
    directional_light_->setDiffuseColour(0, 0, 0);
    directional_light_->setSpecularColour(0, 0, 0);
    directional_light_->setCastShadows(true);

    directional_light_node_ = world_root_->createChildSceneNode();
    directional_light_node_->setInheritOrientation(false);
    directional_light_node_->setInheritScale(false);
    directional_light_node_->attachObject(directional_light_);
}

void World::start()
{   
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

void World::set_ambient_light(const Color& color, float intensity)
{
    ambient_color_ = color;
    ambient_intensity_ = intensity;
    const auto light_vec = color.to_vector3() * intensity;
    manager_->setAmbientLight(Ogre::ColourValue(light_vec.x, light_vec.y, light_vec.z, 1.0f));
}

void World::set_directional_light(const Color& color, float intensity, const Quaternion& rotation)
{
    const Vector3 light = color.to_vector3() * intensity;
    directional_light_node_->setOrientation(rotation.w, rotation.x, rotation.y, rotation.z);
    directional_light_->setDiffuseColour(light.x, light.y, light.z);
    directional_light_->setSpecularColour(light.x, light.y, light.z);
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
        entity->on_destroy();
        for (auto& component : entity->components_)
        {
            component->on_destroy();
        }

        entity->on_destroyed(entity);
    }
    
    Game::instance_->physics_->destroyPhysicsWorld(physics_world_);
    physics_world_ = nullptr;
    
    Game::instance_->ogre_app_->getRoot()->destroySceneManager(manager_);
    manager_ = nullptr;
}

void World::spawn_entity_internal(const Shared<Entity>& entity)
{
    const auto& rot = entity->transform_.rotation;
    entity->scene_node_ = world_root_->createChildSceneNode(cast_object<Ogre::Vector3>(entity->transform_.location), Ogre::Quaternion(rot.w, rot.x, rot.y, rot.z));
    entity->scene_node_->setInheritOrientation(false);
    entity->scene_node_->setInheritScale(false);
    entity->scene_node_->setScale(cast_object<Ogre::Vector3>(entity->transform_.scale));
    entity->world_ = weak_from_this();
    /*if (entity->is_rigid_body())
    {
        entity->rigid_body_ = physics_world_->createRigidBody(
            reactphysics3d::Transform(
                cast_object<reactphysics3d::Vector3>(entity->transform_.location),
                cast_object<reactphysics3d::Quaternion>(entity->transform_.rotation)
            )
        );
        entity->rigid_body_->setUserData(entity.get());
    }*/
    entities_.add(entity);
    entity->start();
}

void World::do_destroy(const Shared<Entity>& entity)
{
    entity->on_destroy();
    for (auto& component : entity->components_)
    {
        component->on_destroy();
    }

    world_root_->removeChild(entity->scene_node_);

    entity->on_destroyed(entity);
}
