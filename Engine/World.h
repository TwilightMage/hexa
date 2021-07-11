#pragma once

#include "SoundHandle.h"
#include "List.h"
#include "Entity.h"
#include "Quaternion.h"
#include "TimerHandle.h"
#include "Vector3.h"

class AudioChannel;
struct RaycastResult;
class Game;
class Entity;

namespace reactphysics3d
{
    class PhysicsWorld;
}

class EXPORT World : public EnableSharedFromThis<World>
{
    friend Game;

    struct TimerEntry
    {
        float time;
        std::function<void()> func;
    };
    
public:
    bool spawn_entity(const Shared<Entity>& entity, const Vector3& pos, const Quaternion& rot);
    bool spawn_entity(const Shared<Entity>& entity, const Vector3& pos);
    bool spawn_entity(const Shared<Entity>& entity, const Quaternion& rot);
    bool spawn_entity(const Shared<Entity>& entity);

    SoundHandle play_sound(const Shared<Audio>& audio, const Shared<AudioChannel>& channel = nullptr);
    SoundHandle play_sound_3d(const Shared<Audio>& audio, const Vector3& location, const Shared<AudioChannel>& channel = nullptr);

    Shared<const RaycastResult> raycast(const Vector3& from, const Vector3& to) const;
    Shared<const RaycastResult> raycast(const Vector3& from, const Vector3& to, byte16 collision_mask) const;
    List<RaycastResult> raycast_all(const Vector3& from, const Vector3& to, bool sort_by_distance = false) const;
    List<RaycastResult> raycast_all(const Vector3& from, const Vector3& to, byte16 collision_mask, bool sort_by_distance = false) const;

    void start();
    void tick(float delta_time);

    const Set<Shared<Entity>>& get_entities() const;

    float get_time_scale() const;
    void set_time_scale(float val);

    Vector3 get_gravity() const;
    void set_gravity(const Vector3& val) const;

    FORCEINLINE float get_time() const { return time_; }

    TimerHandle delay(float time, std::function<void()> func);

    Quaternion sun_angle = Quaternion(Vector3(0, 45, 45));
    Color ambient_color = Color::white();
    float ambient_intensity = 0.6f;
    Color sun_color = Color::white();
    float sun_intensity = 0.8f;
    
protected:
    virtual void on_start();
    virtual void on_tick(float delta_time);
    virtual void on_close();

private:
    void close();
    void spawn_entity_internal(const Shared<Entity>& entity);

    void do_destroy(const Shared<Entity>& entity);
    
    Set<Shared<Entity>> entities_;
    reactphysics3d::PhysicsWorld* physics_world_;
    float physics_tick_accum_;
    float time_scale_ = 1.0f;

    Map<TimerHandle, TimerEntry> timer_entries_;

    float time_ = 0.0f;
};
