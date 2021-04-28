#pragma once
#include <glm/vec3.hpp>
#include <glm/ext/quaternion_float.hpp>

class WorldObject
{
    friend class World;

public:
    virtual void start();
    
    glm::vec3 position;
    glm::quat rotation;

    class World* get_world() const;

private:
    class World* world;
};
