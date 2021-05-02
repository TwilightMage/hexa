#pragma once

#include "BasicTypes.h"
#include "framework.h"
#include "IRenderable.h"

EXTERN class EXPORT Entity : public IRenderable, public std::enable_shared_from_this<Entity>
{
    friend class World;

public:    
    Weak<World> get_world() const;
    
    virtual void start();
    virtual void on_destroy();

    void destroy();

    bool is_started() const;

    virtual void use_mesh(const Weak<class Mesh>& new_mesh) override;
    virtual void clear_mesh() override;
    virtual Weak<class Mesh> get_mesh() const override;
    virtual Weak<class Shader> get_shader() const override;
    virtual Vector3 get_position() const override;
    virtual Quaternion get_rotation() const override;
    virtual Vector3 get_scale() const override;
    
    Vector3 position;
    Quaternion rotation;
    Vector3 scale = Vector3::one();

private:
    Weak<World> world_;
    Shared<class Mesh> mesh_;
    bool pending_kill_;
    bool started_;
};
