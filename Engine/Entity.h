#pragma once

#include "BasicTypes.h"
#include "framework.h"
#include "IRenderable.h"

EXTERN class EXPORT Entity : public IRenderable, public std::enable_shared_from_this<Entity>
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
    virtual Shared<Mesh> get_mesh() const override;
    virtual Shared<Shader> get_shader() const override;
    virtual Shared<Texture> get_texture() const override;
    virtual Vector3 get_position() const override;
    virtual Quaternion get_rotation() const override;
    virtual Vector3 get_scale() const override;
    
    Vector3 position;
    Quaternion rotation;
    Vector3 scale = Vector3::one();

private:
    Weak<World> world_;
    Shared<Mesh> mesh_;
    Shared<Shader> shader_;
    Shared<Texture> texture_;
    bool pending_kill_;
    bool started_;
};
