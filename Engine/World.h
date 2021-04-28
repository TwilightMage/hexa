#pragma once
#include "List.h"
#include "WorldObject.h"

class World
{
public:
    template<typename T>
    T* spawn(glm::vec3 pos, glm::quat rot)
    {
        T* new_obj = new T();
        new_obj->position = pos;
        new_obj->rotation - rot;
        setup_spawn(new_obj);
        return new_obj;
    }

    void setup_spawn(WorldObject* object);

    void start();
    void tick(float delta_time);

protected:
    virtual void on_start();
    virtual void on_tick();

private:
    List<WorldObject*> objects;
};
