﻿#pragma once

#include "framework.h"
#include "Map.h"
#include "Matrix4x4.h"
#include "Mesh.h"
#include "MaterialParameters.h"
#include "Shader.h"
#include "SimpleMap.h"

class Shader;
class MaterialParameterBase;
class World;
class MaterialInstance;

class EXPORT Material : public std::enable_shared_from_this<Material>
{
    friend MaterialInstance;
    
    struct MeshContainer
    {
        uint position_in_buffer = 0;
        uint size_in_buffer = 0;

        List<Shared<MaterialInstance>> active_instances_;
        List<Shared<MaterialInstance>> disabled_instances_;

        void add_instance(const Shared<MaterialInstance>& instance);
        void remove_instance(const Shared<MaterialInstance>& instance);
        void state_changed(const Shared<MaterialInstance>& instance);

        FORCEINLINE uint count() const { return active_instances_.length() + disabled_instances_.length(); }
    };
    
    struct VertexBufferContainer
    {
        void init();
        void cleanup() const;
        void add_mesh(const Shared<Mesh>& mesh, const Shared<MaterialInstance>& first_instance);
        void remove_mesh(const Shared<Mesh>& mesh);

        uint cached_buffer_size = 0;
        SimpleMap<Shared<Mesh>, MeshContainer*> mesh_containers;
        uint gl_id;
    };
    
public:
    struct InputMapping
    {
        void* start;
        uint stride;
    };
    
    struct RenderData
    {
        Shared<World> world;

        Matrix4x4 view_3d;
        Matrix4x4 projection_3d;

        Matrix4x4 projection_2d;
    };

    bool init(const Shared<Shader>& shader, float z_order);
    
    void render(const RenderData& render_data) const;
    void cleanup() const;

    Shared<MaterialInstance> create_instance();
    void destroy_instance(const Shared<MaterialInstance>& instance);

    template<typename T>
    void set_param_value(const String& name, const T& value) const
    {
        for (auto& basic_param : global_parameters_)
        {
            if (basic_param->name == name)
            {
                if (auto param = cast<MaterialParameter<T>>(basic_param))
                {
                    param->value = value;
                    return;
                }
                print_warning("Material", "Attempting to assign a value to a parameter %s of a different type", name.c());
            }
        }
        print_warning("Material", "Attempting to assign a value to a parameter %s which doesn't exists", name.c());
    }

    FORCEINLINE const Shared<Shader>& get_shader() const { return shader_; }

protected:
    template<typename T>
    Shared<MaterialParameter<T>> get_parameter(const String& name) const
    {
        for (auto& param : global_parameters_)
        {
            if (param->name == name) return cast<MaterialParameter<T>>(param);
        }
        
        return nullptr;
    }
    
    virtual bool verify_shader_valid(const Shared<Shader>& shader) = 0;
    virtual void register_direct_parameters();
    virtual void apply_params(const RenderData& render_data) const;

    virtual Shared<MaterialInstance> create_instance_object();

private:
    void change_mesh(const Shared<MaterialInstance>& instance, const Shared<Mesh>& old_mesh, const Shared<Mesh>& new_mesh);
    void change_active(const Shared<MaterialInstance>& instance);
    
    List<VertexBufferContainer*> vertex_buffers_;
    List<Shared<MaterialInstance>> empty_instances_;
    Map<Shared<Mesh>, uint> mesh_buffer_map_;
    Shared<Shader> shader_ = nullptr;
    List<Shared<MaterialParameterBase>> global_parameters_;
    bool is_valid_ = false;

    static const inline int min_buffer_size = 1024 * 1024 * 1;
    static const inline int max_buffer_size = 1024 * 1024 * 4;
};

