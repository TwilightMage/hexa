#pragma once
#include "List.h"
#include "Entity.h"

class Game;

EXTERN class EXPORT World : std::enable_shared_from_this<World>
{
    friend Game;
    
public:
    void spawn_entity(const Weak<Entity>& entity, glm::vec3 pos, glm::quat rot);

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
