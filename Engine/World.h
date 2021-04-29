#pragma once
#include "List.h"
#include "Entity.h"

class World
{
public:
    template<typename T>
    Shared<T> spawn(glm::vec3 pos, glm::quat rot)
    {
        Shared<T> entity = MakeShared<T>();
        entity->position = pos;
        entity->rotation = rot;
        setup_spawn(entity);
        return entity;
    }

    void setup_spawn(const Weak<Entity>& entity);

    void start();
    void tick(float delta_time);

    const List<Shared<Entity>>& get_entities() const;

    static void notify_renderable_added(const Weak<IRenderable>& renderable);
    static void notify_renderable_deleted(const Weak<IRenderable>& renderable);
    static void notify_renderable_updated(const Weak<IRenderable>& renderable, const Weak<Mesh>& old_mesh);

protected:
    virtual void on_start();
    virtual void on_tick();

private:
    List<Shared<Entity>> entities_;
};
