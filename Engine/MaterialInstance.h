#pragma once

#include "BasicTypes.h"
#include "Logger.h"
#include "Material.h"
#include "MaterialParameters.h"
#include "Shader.h"
#include "String.h"

class Material;
class Mesh;
class Shader;

class EXPORT MaterialInstance : public std::enable_shared_from_this<MaterialInstance>
{
    friend Material;
    
public:    
    FORCEINLINE Shared<Material> get_material() const { return material_.lock(); }

    FORCEINLINE Shared<Mesh> get_mesh() const { return mesh_; }
    void set_mesh(const Shared<Mesh>& mesh);
    
    FORCEINLINE bool is_visible() const { return visible_; }
    void set_visible(bool visible);

    template<typename T>
    void set_param_value(const String& name, const T& value)
    {
        String shader_name = "NULL";
        if (const auto material = get_material())
        {
            if (material->shader_)
            {
                shader_name = material->shader_->name;
            }
        }
        
        for (auto& basic_param : instance_parameters_)
        {
            if (basic_param->name == name)
            {
                if (auto param = cast<MaterialParameter<T>>(basic_param))
                {
                    param->value = value;
                    return;
                }
                print_warning("Material", "Attempting to assign a %s value to instance parameter %s %s on shader %s", typeid(T).name(), basic_param->type->name.c(), name.c(), shader_name.c());
                return;
            }
        }
        print_warning("Material", "Attempting to assign a value to instance parameter %s which doesn't exists on shader %s", name.c(), shader_name.c());
    }

    template<typename T>
    Shared<MaterialParameter<T>> get_parameter(const String& name) const
    {
        for (auto& param : instance_parameters_)
        {
            if (param->name == name) return cast<MaterialParameter<T>>(param);
        }
        
        return nullptr;
    }

    void copy_parameters_from(const Shared<MaterialInstance>& another_instance);

    void destroy();

protected:
    virtual void register_direct_parameters();

private:
    Weak<Material> material_;
    Shared<Mesh> mesh_;
    bool visible_ = true;

    List<Shared<MaterialParameterBase>> instance_parameters_;
};
