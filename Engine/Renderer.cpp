#include "Renderer.h"

#include <glad/glad.h>

#include "Game.h"
#include "RendererInstance.h"
#include "Shader.h"
#include "Utils.h"
#include "World.h"

template<typename T>
FORCEINLINE T* extract_gl_buffer(uint buffer, uint buffer_type, int& buffer_size, int add_size = 0)
{
    glBindBuffer(buffer_type, buffer);

    int src_size;
    glGetBufferParameteriv(buffer_type, GL_BUFFER_SIZE, &src_size);
    
    buffer_size = src_size / sizeof(T) + add_size;

    T* buffer_data = new T[buffer_size];

    if (src_size > 0)
    {
        void* buf_ptr = glMapBuffer(buffer_type, GL_READ_ONLY);
        memcpy(buffer_data, buf_ptr, sizeof(T) * (buffer_size - add_size));
        glUnmapBuffer(buffer_type);
    }

    glBindBuffer(buffer_type, 0);
    
    return buffer_data;
}

void Renderer::MeshContainer::add_instance(const Shared<RendererInstance>& instance)
{
    if (instance->is_visible())
    {
        active_instances_.Add(instance);
    }
    else
    {
        disabled_instances_.Add(instance);
    }
}

void Renderer::MeshContainer::remove_instance(const Shared<RendererInstance>& instance)
{
    if (instance->is_visible())
    {
        active_instances_.Remove(instance);
    }
    else
    {
        disabled_instances_.Remove(instance);
    }
}

void Renderer::MeshContainer::state_changed(const Shared<RendererInstance>& instance)
{
    if (instance->is_visible())
    {
        disabled_instances_.Remove(instance);
        active_instances_.Add(instance);
    }
    else
    {
        active_instances_.Remove(instance);
        disabled_instances_.Add(instance);
    }
}

void Renderer::VertexBufferContainer::init()
{
    glGenBuffers(1, &gl_id);
}

void Renderer::VertexBufferContainer::cleanup() const
{
    glDeleteBuffers(1, new uint[1] { gl_id });
}

void Renderer::VertexBufferContainer::add_mesh(const Shared<Mesh>& mesh, const Shared<RendererInstance>& first_instance)
{
    int current_buffer_size;
    Mesh::Vertex* current_buffer_data = extract_gl_buffer<Mesh::Vertex>(gl_id, GL_ARRAY_BUFFER, current_buffer_size, mesh->get_vertices().length());
    const uint new_start = current_buffer_size - mesh->get_vertices().length();
    memcpy(current_buffer_data + new_start, mesh->get_vertices().get_data(), sizeof(Mesh::Vertex) * mesh->get_vertices().length());

    glBindBuffer(GL_ARRAY_BUFFER, gl_id);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Mesh::Vertex) * current_buffer_size, current_buffer_data, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    cached_buffer_size = current_buffer_size;
    delete current_buffer_data;
    MeshContainer* new_mesh_container = new MeshContainer();
    new_mesh_container->position_in_buffer = new_start;
    new_mesh_container->size_in_buffer = mesh->get_vertices().length();
    if (first_instance->is_visible())
    {
        new_mesh_container->active_instances_.Add(first_instance);
    }
    else
    {
        new_mesh_container->disabled_instances_.Add(first_instance);
    }
    mesh_containers.insert(mesh, new_mesh_container);
}

void Renderer::VertexBufferContainer::remove_mesh(const Shared<Mesh>& mesh)
{
    int current_buffer_size;
    Mesh::Vertex* current_buffer_data = extract_gl_buffer<Mesh::Vertex>(gl_id, GL_ARRAY_BUFFER, current_buffer_size, mesh->get_vertices().length());
    const uint offset = mesh->get_vertices().length();
    const uint from = mesh_containers[mesh]->position_in_buffer;
    delete mesh_containers[mesh];
    mesh_containers.remove(mesh);
    for (auto& container : mesh_containers)
    {
        if (container.value->position_in_buffer > from)
        {
            container.value->position_in_buffer -= offset;
        }
    }
    memcpy(current_buffer_data + from, current_buffer_data + from + offset, sizeof(Mesh::Vertex) * (current_buffer_size - from - offset));
    
    glBindBuffer(GL_ARRAY_BUFFER, gl_id);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Mesh::Vertex) * current_buffer_size - offset, current_buffer_data, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    cached_buffer_size = current_buffer_size - offset;
    delete current_buffer_data;
}

bool Renderer::init(const Shared<Shader>& shader)
{
    if (is_valid_) return shader_ == shader;
    
    if (verify_shader_valid(shader))
    {
        shader_ = shader;

        for (auto& uniform_parameter : shader->global_uniforms_)
        {
            global_parameters_.insert(uniform_parameter.value.name, uniform_parameter.value.type->parameter_producer());
        }

        register_direct_parameters();

        if (shader_->empty_vertex_ > 0)
        {
            VertexBufferContainer* vbc = new VertexBufferContainer();
            vbc->init();
            vbc->mesh_containers.insert(nullptr, new MeshContainer());
            vertex_buffers_.Add(vbc);
            mesh_buffer_map_.insert(nullptr, 0);
        }
        
        Game::instance_->renderers_.Add(shared_from_this());
        return is_valid_ = true;
    }
    else
    {
        print_error("Renderer", "Failed to initialize renderer with shader %s because of bad signature", shader->name.c());
    }

    return is_valid_ = false;
}

void Renderer::render(const RenderData& render_data) const
{
    apply_params(render_data);
    
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    
    glEnable(GL_CULL_FACE);
    glDisable(GL_CULL_FACE);
    glCullFace(GL_FRONT);

    if (shader_->transparency_)
    {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    }
    else
    {
        glDisable(GL_BLEND);
    }
    glUseProgram(shader_->get_program());

    for (auto& vertex_buffer_container : vertex_buffers_)
    {
        glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_container->gl_id);
        for (auto& input : shader_->vertex_params_)
        {
            uint layout = input.layout;
            glEnableVertexAttribArray(layout);
            glVertexAttribPointer(layout, input.type->gl_size, (GLenum)input.type->gl_primitive_type, GL_FALSE, sizeof(Mesh::Vertex), input.offset);
        }

        for (const auto& mesh_container_point : vertex_buffer_container->mesh_containers)
        {
            auto mesh_container = mesh_container_point.value;
            
            uint rendered_instance_count = 0;
            while (rendered_instance_count < mesh_container->active_instances_.length())
            {
                const auto instance_count = shader_->instance_count_ == 0 ? mesh_container->active_instances_.length() : std::min(mesh_container->active_instances_.length() - rendered_instance_count, shader_->instance_count_);

                List<RendererParameterApplier*> parameter_appliers;

                for (auto& shader_instance_param : get_shader()->instance_uniforms_)
                {
                    parameter_appliers.Add(new RendererParameterApplier(instance_count,  shader_instance_param.value.type, shader_instance_param.value.layout));
                }
                
                for (uint j = 0; j < instance_count; j++)
                {
                    uint i = 0;
                    for (auto& instance_parameter : mesh_container->active_instances_[rendered_instance_count + j]->instance_parameters_)
                    {
                        parameter_appliers[i++]->put(instance_parameter.value.get(), j);
                    }
                }
    
                for (uint i = 0; i < parameter_appliers.length(); i++)
                {
                    parameter_appliers[i]->apply();
                    delete parameter_appliers[i];
                }
                parameter_appliers.Clear();

                for (auto& shader_global_param : get_shader()->global_uniforms_)
                {
                    parameter_appliers.Add(new RendererParameterApplier(1,  shader_global_param.value.type, shader_global_param.value.layout));
                }
                
                uint i = 0;
                for (auto& global_parameter : global_parameters_)
                {
                    parameter_appliers[i++]->put(global_parameter.value.get(), 0);
                }
    
                for (uint i = 0; i < parameter_appliers.length(); i++)
                {
                    parameter_appliers[i]->apply();
                    delete parameter_appliers[i];
                }

                if (shader_->empty_vertex_ > 0)
                {
                    glDrawArraysInstanced(GL_TRIANGLES, 0, shader_->empty_vertex_, instance_count);
                }
                else
                {
                    glDrawArraysInstanced(GL_TRIANGLES, mesh_container->position_in_buffer, mesh_container->size_in_buffer, instance_count);
                }
                rendered_instance_count += instance_count;
            }
        }
    }
}

void Renderer::cleanup() const
{
    //mesh_instances_->cleanup();
}

Shared<RendererInstance> Renderer::create_instance()
{
    Shared<RendererInstance> instance = create_instance_object();
    instance->master_renderer_ = weak_from_this();
    for (auto& uniform_parameter : shader_->instance_uniforms_)
    {
        instance->instance_parameters_.insert(uniform_parameter.value.name, uniform_parameter.value.type->parameter_producer());
    }
    instance->register_direct_parameters();
    if (shader_->empty_vertex_ > 0)
    {
        vertex_buffers_[0]->mesh_containers[nullptr]->add_instance(instance);
    }
    else
    {
        empty_instances_.Add(instance);
    }
    return instance;
}

void Renderer::destroy_instance(const Shared<RendererInstance>& instance)
{
    if (instance->get_master_renderer().get() == this)
    {
        if (instance->mesh_ == nullptr && shader_->empty_vertex_ == 0)
        {
            empty_instances_.Remove(instance);
        }
        else
        {
            if (const auto found = mesh_buffer_map_.find(instance->mesh_))
            {
                auto vertex_buffer_container = vertex_buffers_[*found];
                auto mesh_container = vertex_buffer_container->mesh_containers[instance->mesh_];
                
                mesh_container->remove_instance(instance);

                if (mesh_container->count() == 0 && shader_->empty_vertex_ == 0)
                {
                    vertex_buffer_container->remove_mesh(instance->mesh_);
                    mesh_buffer_map_.remove(instance->mesh_);
                }
            }
        }
        
        instance->master_renderer_ = null_weak(Renderer);
    }
}

void Renderer::register_direct_parameters()
{
}

void Renderer::apply_params(const RenderData& render_data) const
{
}

Shared<RendererInstance> Renderer::create_instance_object()
{
    return MakeShared<RendererInstance>();
}

void Renderer::change_mesh(const Shared<RendererInstance>& instance, const Shared<Mesh>& old_mesh, const Shared<Mesh>& new_mesh)
{
    if (instance->get_master_renderer().get() == this && old_mesh != new_mesh)
    {
        if (old_mesh == nullptr)
        {
            empty_instances_.Remove(instance);
            if (auto found = mesh_buffer_map_.find(new_mesh)) // case - no mesh -> registered mesh
            {
                vertex_buffers_[*found]->mesh_containers[old_mesh]->add_instance(instance);
            }
            else // case - no mesh -> unregistered mesh
            {
                uint use_buffer_index = -1;
                const int mesh_size = (int)new_mesh->get_vertices().length() * (int)sizeof(Mesh::Vertex);
                for (uint i = 0; i < vertex_buffers_.length(); i++)
                {
                    if (max_buffer_size - (int)vertex_buffers_[i]->cached_buffer_size * (int)sizeof(Mesh::Vertex) > mesh_size)
                    {
                        vertex_buffers_[i]->add_mesh(new_mesh, instance);
                        use_buffer_index = i;
                        break;
                    }
                }
                
                if (use_buffer_index == -1)
                {
                    VertexBufferContainer* vbc = new VertexBufferContainer();
                    vbc->init();
                    vbc->add_mesh(new_mesh, instance);
                    use_buffer_index = 0;
                    vertex_buffers_.Add(vbc);
                }
                mesh_buffer_map_.insert(new_mesh, use_buffer_index);
            }
        }
        else if (new_mesh == nullptr) // case - registered mesh -> no mesh
        {
            if (const auto found = mesh_buffer_map_.find(old_mesh))
            {
                auto vertex_buffer_container = vertex_buffers_[*found];
                auto mesh_container = vertex_buffer_container->mesh_containers[old_mesh];
                
                mesh_container->remove_instance(instance);

                if (mesh_container->count() == 0)
                {
                    vertex_buffer_container->remove_mesh(old_mesh);
                    mesh_buffer_map_.remove(old_mesh);
                }
            }
            empty_instances_.Add(instance);
        }
    }
}

void Renderer::change_active(const Shared<RendererInstance>& instance)
{
    if (instance->get_master_renderer().get() == this)
    {
        if (const auto found = mesh_buffer_map_.find(instance->get_mesh()))
        {
            vertex_buffers_[*found]->mesh_containers[instance->get_mesh()]->state_changed(instance);
        }
    }
}
