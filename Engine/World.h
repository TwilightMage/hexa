#pragma once
#include "List.h"
#include "Entity.h"
#include "Quaternion.h"
#include "Vector3.h"

class Game;

EXTERN class EXPORT World : public std::enable_shared_from_this<World>
{
    friend Game;
    
public:
    void spawn_entity(const Weak<Entity>& entity, const Vector3& pos, const Quaternion& rot = Quaternion());

    void start();
    void tick(float delta_time);

    const List<Shared<Entity>>& get_entities() const;

    static void notify_renderable_added(const Weak<IRenderable>& renderable);
    static void notify_renderable_deleted(const Weak<IRenderable>& renderable);
    static void notify_renderable_updated(const Weak<IRenderable>& renderable, const Weak<Mesh>& old_mesh);

protected:
    virtual void on_start();
    virtual void on_tick();
    virtual void on_close();

private:
    List<Shared<Entity>> entities_;
};
