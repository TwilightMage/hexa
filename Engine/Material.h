#pragma once

#include "framework.h"
#include "Map.h"
#include "Matrix4x4.h"
#include "Mesh.h"
#include "MaterialParameters.h"
#include "Set.h"
#include "Shader.h"
#include "SimpleMap.h"

class Shader;
class MaterialParameterBase;
class World;
class MaterialInstance;

class EXPORT Material : public Object, public EnableSharedFromThis<Material>
{
    friend MaterialInstance;

public:
    struct MeshContainer
    {
        uint position_in_buffer = 0;
        uint size_in_buffer = 0;

        Set<Shared<MaterialInstance>> active_instances_;
        Set<Shared<MaterialInstance>> disabled_instances_;

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
    
    struct RenderData
    {
        Shared<World> world;

        Matrix4x4 view_3d;
        Matrix4x4 projection_3d;

        Matrix4x4 projection_2d;
    };

    Material();
    ~Material();

    void init(const Shared<Shader>& shader, float z_order);
    
    void render(const RenderData& render_data) const;
    void cleanup() const;

    Shared<MaterialInstance> create_instance();
    void destroy_instance(const Shared<MaterialInstance>& instance);

    template<typename T>
    void set_param_value(const Name& name, const T& value) const
    {
        String shader_name = "NULL";
        if (shader_)
        {
            shader_name = shader_->name;
        }
        
        if (auto basic_param = global_parameters_.find(name))
        {
            if (auto param = cast<MaterialParameter<T>>(*basic_param))
            {
                param->value = value;
                return;
            }
            print_warning("Material", "Attempting to assign a %s value to global parameter %s %s on shader %s", typeid(T).name(), (*basic_param)->type->name.c(), name.to_string().c(), shader_name.c());
            return;
        }
        print_warning("Material", "Attempting to assign a value to global parameter %s which doesn't exists on shader %s", name.to_string().c(), shader_name.c());
    }

    template<typename T>
    Shared<MaterialParameter<T>> get_parameter(const Name& name) const
    {
        if (auto param = global_parameters_.find(name))
        {
            return cast<MaterialParameter<T>>(*param);
        }
        
        return nullptr;
    }

    FORCEINLINE const Shared<Shader>& get_shader() const { return shader_; }

protected:
    virtual void register_direct_parameters();
    virtual void apply_params(const RenderData& render_data) const;

    virtual Shared<MaterialInstance> create_instance_object();

private:
    void change_mesh(const Shared<MaterialInstance>& instance, const Shared<Mesh>& old_mesh, const Shared<Mesh>& new_mesh);
    void change_active(const Shared<MaterialInstance>& instance);

    List<VertexBufferContainer*> vertex_buffers_;
    Set<Shared<MaterialInstance>> empty_instances_;
    SimpleMap<Shared<Mesh>, uint> mesh_buffer_map_;
    Shared<Shader> shader_ = nullptr;
    SimpleMap<Name, Shared<MaterialParameterBase>> global_parameters_;
    bool is_valid_ = false;

    List<void*> global_batch;

    static const inline int min_buffer_size = 1024 * 1024 * 1;
    static const inline int max_buffer_size = 1024 * 1024 * 1;
};


