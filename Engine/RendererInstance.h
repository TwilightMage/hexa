#pragma once

#include "BasicTypes.h"
#include "RendererParameters.h"
#include "Shader.h"
#include "String.h"

class Shader;

class RendererInstance : public std::enable_shared_from_this<RendererInstance>
{
    friend Renderer;
    
public:
    FORCEINLINE Shared<Renderer> get_master_renderer() const { return master_renderer_.lock(); }

    FORCEINLINE Shared<Mesh> get_mesh() const { return mesh_; }
    void set_mesh(const Shared<Mesh>& mesh);
    
    FORCEINLINE bool is_visible() const { return visible_; }
    void set_visible(bool visible);

    template<typename T>
    void set_param(const String& name, const T& value)
    {
        if (const auto& basic_param = instance_parameters_.find(name))
        {
            if (auto param = cast<RendererParameter<T>>(*basic_param))
            {
                param->value = value;
                return;
            }
            print_warning("Renderer", "Attempting to assign a value to a parameter %s of a different type", name.c());
        }
        print_warning("Renderer", "Attempting to assign a value to a parameter %s which doesn't exists", name.c());
    }

    void destroy();

protected:
    template<typename T>
    Shared<RendererParameter<T>> get_parameter(const String& name) const
    {
        return cast<RendererParameter<T>>(instance_parameters_.find_or_default(name));
    }

    virtual void register_direct_parameters();

private:
    Weak<Renderer> master_renderer_;
    Shared<Mesh> mesh_;
    bool visible_ = true;

    Map<String, Shared<RendererParameterBase>> instance_parameters_;
};
