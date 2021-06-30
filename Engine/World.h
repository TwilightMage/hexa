#pragma once

#include "List.h"
#include "Entity.h"
#include "Quaternion.h"
#include "TimerHandle.h"
#include "Vector3.h"

struct RaycastResult;
class Game;
class Entity;

namespace reactphysics3d
{
    class PhysicsWorld;
}

class EXPORT World : public std::enable_shared_from_this<World>
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

    Shared<const RaycastResult> raycast(const Vector3& from, const Vector3& to) const;
    Shared<const RaycastResult> raycast(const Vector3& from, const Vector3& to, byte16 collision_mask) const;
    List<RaycastResult> raycast_all(const Vector3& from, const Vector3& to, bool sort_by_distance = false) const;
    List<RaycastResult> raycast_all(const Vector3& from, const Vector3& to, byte16 collision_mask, bool sort_by_distance = false) const;

    void start();
    void tick(float delta_time);

    const List<Shared<Entity>>& get_entities() const;

    static void notify_renderable_added(const Shared<IRenderable>& renderable);
    static void notify_renderable_deleted(const Shared<IRenderable>& renderable);
    static void notify_renderable_mesh_updated(const Shared<IRenderable>& renderable, const Shared<Mesh>& old_mesh);
    static void notify_renderable_shader_updated(const Shared<IRenderable>& renderable, const Shared<Shader>& old_shader);

    float get_time_scale() const;
    void set_time_scale(float val);

    Vector3 get_gravity() const;
    void set_gravity(const Vector3& val) const;

    TimerHandle delay(float time, std::function<void()> func);

    FORCEINLINE const Quaternion& get_sun_angle() const { return sun_angle_; }
    void set_sun_pitch(float sun_pitch);
    void set_sun_yaw(float sun_yaw);

    Color ambient_color_ = Color::white();
    float ambient_intensity_ = 0.5f;
    Color sun_color_ = Color::white();
    float sun_intensity_ = 0.8f;
    
protected:
    virtual void on_start();
    virtual void on_tick();
    virtual void on_close();

private:
    void close();
    void spawn_entity_internal(const Shared<Entity>& entity);
    bool ensure_child_not_exist(const Shared<Entity>& entity);

    void do_destroy(const Shared<Entity>& entity);
    
    List<Shared<Entity>> entities_;
    reactphysics3d::PhysicsWorld* physics_world_;
    float physics_tick_accum_;
    float time_scale_ = 1.0f;

    Map<TimerHandle, TimerEntry> timer_entries_;

    Quaternion sun_angle_ = Quaternion(Vector3(0, 45, 0));
};
