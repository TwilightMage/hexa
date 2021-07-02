#pragma once

#include "framework.h"
#include "Map.h"
#include "Matrix4x4.h"
#include "Mesh.h"
#include "Shader.h"

class RendererParameterBase;
class World;
class RendererInstance;

class EXPORT Renderer
{
    friend RendererInstance;
    
    struct MeshContainer
    {
        uint position_in_buffer;
        uint size_in_buffer;

        List<Shared<RendererInstance>> active_instances_;
        List<Shared<RendererInstance>> disabled_instances_;

        void add_instance(const Shared<RendererInstance>& instance);
        void remove_instance(const Shared<RendererInstance>& instance);
        void state_changed(const Shared<RendererInstance>& instance);

        FORCEINLINE uint count() const { return active_instances_.length() + disabled_instances_.length(); }
    };
    
    struct VertexBufferContainer
    {
        void init();
        void cleanup() const;
        void add_mesh(const Shared<Mesh>& mesh, const Shared<RendererInstance>& first_instance);
        void remove_mesh(const Shared<Mesh>& mesh);

        List<Mesh::Vertex> vertex_buffer;
        Map<Shared<Mesh>, MeshContainer*> mesh_containers;
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

        Matrix4x4 view_2d;
        Matrix4x4 projection_2d;
    };
    
    void render(const RenderData& render_data) const;
    void cleanup() const;

    Shared<RendererInstance> create_instance();
    void destroy_instance(const Shared<RendererInstance>& instance);

    template<typename T>
    void set_param_value(const String& name, const T& value)
    {
        if (const auto& basic_param = global_parameters_.find(name))
        {
            if (auto param = cast<RendererParameter<T>>(*basic_param))
            {
                param->value = value;
            }
        }
    }

    FORCEINLINE const Shared<Shader>& get_shader() const { return shader_; }

protected:
    virtual bool verify_shader_valid(const Shared<Shader>& shader) = 0;

    virtual Shared<RendererInstance> create_instance_object();

private:
    template<class RendererClass>
    static Shared<RendererClass> create(const Shared<Shader>& shader)
    {
        Shared<RendererClass> renderer = MakeShared<RendererClass>();
        if (renderer->verify_shader_valid(shader))
        {
            renderer->renderer_instance_ = shader;

            for (auto& uniform_parameter : shader->global_uniforms_)
            {
                renderer->global_parameters_.insert(uniform_parameter.value.name, shader_type_info[uniform_parameter.value.type].parameter_producer());
            }
            
            return renderer;
        }

        return nullptr;
    }
    
    void change_mesh(const Shared<RendererInstance>& instance, const Shared<Mesh>& old_mesh, const Shared<Mesh>& new_mesh);
    void change_active(const Shared<RendererInstance>& instance);
    
    List<VertexBufferContainer*> vertex_buffers_;
    List<Shared<RendererInstance>> empty_instances_;
    Map<Shared<Mesh>, uint> mesh_buffer_map_;
    Shared<Shader> shader_ = nullptr;
    Map<String, Shared<RendererParameterBase>> global_parameters_;

    static const inline int min_buffer_size = 1024 * 1024 * 1;
    static const inline int max_buffer_size = 1024 * 1024 * 4;
};


