#pragma once
#include "List.h"
#include "Entity.h"

class World
{
public:
    template<typename T>
    T* spawn(glm::vec3 pos, glm::quat rot)
    {
        T* entity = new T();
        entity->position = pos;
        entity->rotation - rot;
        setup_spawn(entity);
        return entity;
    }

    void setup_spawn(Entity* entity);

    void start();
    void tick(float delta_time);

    const List<Shared<Entity>>& get_entities() const;

    static void notify_renderable_added(class IRenderable* renderable);
    static void notify_renderable_updated(class IRenderable* renderable, Weak<class Mesh> old_mesh);

protected:
    virtual void on_start();
    virtual void on_tick();

private:
    List<Shared<Entity>> entities_;
};
