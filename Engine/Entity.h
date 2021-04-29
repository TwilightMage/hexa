﻿#pragma once
#include <glm/vec3.hpp>
#include <glm/ext/quaternion_float.hpp>

#include "BasicTypes.h"
#include "IRenderable.h"

class Entity : public IRenderable
{
    friend class World;

public:
    class World* get_world() const;
    
    virtual void start();

    void destroy();

    bool is_started() const;

    virtual void use_mesh(class Mesh* new_mesh) override;
    virtual void clear_mesh() override;
    virtual Weak<class Mesh> get_mesh() const override;
    virtual Weak<class Shader> get_shader() const override;
    virtual glm::vec3 get_position() override;
    virtual glm::quat get_rotation() override;
    
    glm::vec3 position;
    glm::quat rotation;

private:
    class World* world_;
    Shared<class Mesh> mesh_;
    bool pending_kill_;
    bool started_;
};