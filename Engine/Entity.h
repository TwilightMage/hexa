#pragma once

#include "BasicTypes.h"
#include "framework.h"
#include "IRenderable.h"
#include "Object.h"
#include "String.h"

EXTERN class EXPORT Entity : public Object, public IRenderable, public std::enable_shared_from_this<Entity>
{
    friend class World;

public:
    Entity();
    
    Weak<World> get_world() const;
    
    virtual void on_start();
    virtual void on_destroy();

    void destroy();

    bool is_started() const;

    void use_mesh(const Weak<class Mesh>& new_mesh);
    void clear_mesh();
    void use_shader(const Weak<Shader>& new_shader);
    void clear_shader();
    void use_texture(const Weak<Texture>& new_texture);
    void clear_texture();
    Shared<Mesh> get_mesh() const override;
    Shared<Shader> get_shader() const override;
    Shared<Texture> get_texture() const override;
    Vector3 get_position() const override;
    Quaternion get_rotation() const override;
    Vector3 get_scale() const override;
    
    Vector3 position;
    Quaternion rotation;
    Vector3 scale = Vector3::one();

private:
    void start();
    bool should_use_texture() const;
    
    Weak<World> world_;
    Shared<Mesh> mesh_;
    Shared<Shader> shader_;
    Shared<Texture> texture_;
    bool pending_kill_;
    bool started_;
};
