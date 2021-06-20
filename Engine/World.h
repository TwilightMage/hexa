#pragma once

#include "List.h"
#include "Entity.h"
#include "Quaternion.h"
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
    
public:
    void spawn_entity(const Shared<Entity>& entity, const Vector3& pos, const Quaternion& rot);
    void spawn_entity(const Shared<Entity>& entity, const Vector3& pos);
    void spawn_entity(const Shared<Entity>& entity, const Quaternion& rot);
    void spawn_entity(const Shared<Entity>& entity);

    Shared<const RaycastResult> raycast(const Vector3& from, const Vector3& to) const;
    Shared<const RaycastResult> raycast(const Vector3& from, const Vector3& to, byte16 collision_mask) const;

    void start();
    void tick(float delta_time);

    const List<Shared<Entity>>& get_entities() const;

    static void notify_renderable_added(const Weak<IRenderable>& renderable);
    static void notify_renderable_deleted(const Weak<IRenderable>& renderable);
    static void notify_renderable_mesh_updated(const Weak<IRenderable>& renderable, const Weak<Mesh>& old_mesh);
    static void notify_renderable_shader_updated(const Weak<IRenderable>& renderable, const Weak<Shader>& old_shader);

    float get_time_scale() const;
    void set_time_scale(float val);

    Vector3 get_gravity() const;
    void set_gravity(const Vector3& val) const;

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
};
