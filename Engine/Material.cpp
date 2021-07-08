#include "Material.h"

#include <glad/glad.h>

#include "Assert.h"
#include "Game.h"
#include "MaterialInstance.h"
#include "Shader.h"
#include "World.h"

FORCEINLINE void apply_uniform(GLTypeEnum gl_type, uint location, uint count, void* data)
{
    switch (gl_type)
    {
    case GLTypeEnum::Int:
        glUniform1iv(location, count, (int*)data);
        break;
                
    case GLTypeEnum::Uint:
        glUniform1uiv(location, count, (uint*)data);
        break;
                
    case GLTypeEnum::Bool:
#ifdef glUniform1bv
        glUniform1bv(location, count, (bool*)data);
#endif
        break;

    case GLTypeEnum::Float:
        glUniform1fv(location, count, (float*)data);
        break;

    case GLTypeEnum::Vec2:
        glUniform2fv(location, count, (float*)data);
        break;

    case GLTypeEnum::Vec3:
        glUniform3fv(location, count, (float*)data);
        break;

    case GLTypeEnum::Vec4:
        glUniform4fv(location, count, (float*)data);
        break;

    case GLTypeEnum::Mat4:
        glUniformMatrix4fv(location, count, GL_FALSE, (float*)data);
        break;

    case GLTypeEnum::Sampler2D:
        glUniformHandleui64vARB(location, count, (uint64*)data);
        break;
    case GLTypeEnum::SamplerCube:
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, *(uint*)data);
        break;
    default:
        break;
    }
}

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

void Material::MeshContainer::add_instance(const Shared<MaterialInstance>& instance)
{
    if (instance->is_visible())
    {
        active_instances_.add(instance);
    }
    else
    {
        disabled_instances_.add(instance);
    }
}

void Material::MeshContainer::remove_instance(const Shared<MaterialInstance>& instance)
{
    if (instance->is_visible())
    {
        active_instances_.remove(instance);
    }
    else
    {
        disabled_instances_.remove(instance);
    }
}

void Material::MeshContainer::state_changed(const Shared<MaterialInstance>& instance)
{
    if (instance->is_visible())
    {
        disabled_instances_.remove(instance);
        active_instances_.add(instance);
    }
    else
    {
        active_instances_.remove(instance);
        disabled_instances_.add(instance);
    }
}

void Material::VertexBufferContainer::init()
{
    glGenBuffers(1, &gl_id);
}

void Material::VertexBufferContainer::cleanup() const
{
    glDeleteBuffers(1, new uint[1] { gl_id });
}

void Material::VertexBufferContainer::add_mesh(const Shared<Mesh>& mesh, const Shared<MaterialInstance>& first_instance)
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
        new_mesh_container->active_instances_.add(first_instance);
    }
    else
    {
        new_mesh_container->disabled_instances_.add(first_instance);
    }
    mesh_containers[mesh] = new_mesh_container;
}

void Material::VertexBufferContainer::remove_mesh(const Shared<Mesh>& mesh)
{
    int current_buffer_size;
    Mesh::Vertex* current_buffer_data = extract_gl_buffer<Mesh::Vertex>(gl_id, GL_ARRAY_BUFFER, current_buffer_size, mesh->get_vertices().length());
    const uint offset = mesh->get_vertices().length();

    const uint from = mesh_containers[mesh]->position_in_buffer;
    delete mesh_containers[mesh];
    mesh_containers.remove(mesh);
    
    for (auto& container : mesh_containers)
    {
        if (container->value->position_in_buffer > from)
        {
            container->value->position_in_buffer -= offset;
        }
    }
    memcpy(current_buffer_data + from, current_buffer_data + from + offset, sizeof(Mesh::Vertex) * (current_buffer_size - from - offset));
    
    glBindBuffer(GL_ARRAY_BUFFER, gl_id);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Mesh::Vertex) * current_buffer_size - offset, current_buffer_data, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    cached_buffer_size = current_buffer_size - offset;
    delete current_buffer_data;
}

Material::Material()
    : Object("Empty material")
{
}

void Material::init(const Shared<Shader>& shader, float z_order)
{
    if (!shader) return;
    
    if (is_valid_) return;
    
    shader_ = shader;

    for (auto& uniform_parameter : shader->get_global_uniforms())
    {
        global_parameters_.add(uniform_parameter.type->parameter_producer(uniform_parameter.name));
    }

    register_direct_parameters();

    if (shader_->get_empty_vertex() > 0)
    {
        VertexBufferContainer* vbc = new VertexBufferContainer();
        vbc->init();
        vbc->mesh_containers[nullptr] = new MeshContainer();
        vertex_buffers_.add(vbc);
        mesh_buffer_map_[nullptr] = 0;
    }

    name = shader->name + " material";
        
    Game::instance_->materials_[z_order].add(shared_from_this());
    is_valid_ = true;
}

void Material::render(const RenderData& render_data) const
{    
    apply_params(render_data);

    if (shader_->get_no_face_cull())
    {
        glDisable(GL_CULL_FACE);
    }
    else
    {
        glEnable(GL_CULL_FACE);
        glCullFace(GL_FRONT);
    }

    if (shader_->get_transparency())
    {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    }
    else
    {
        glDisable(GL_BLEND);
    }
    
    glUseProgram(shader_->get_program());

    // Global parameters
    uint parameter_count = shader_->get_global_uniforms().length();
                
    for (uint i = 0; i < parameter_count; i++)
    {
        const auto param = shader_->get_global_uniforms()[i];
        void* parameter_buffer = malloc(param.type->c_size);
        global_parameters_[i]->write_data(parameter_buffer);
        apply_uniform(param.type->gl_type, param.layout, 1, parameter_buffer);
        free(parameter_buffer);
    }

    uint dcc = 0;

    for (auto& vertex_buffer_container : vertex_buffers_)
    {
        glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_container->gl_id);
        for (auto& input : shader_->get_vertex_params())
        {
            const uint layout = input.layout;
            glEnableVertexAttribArray(layout);
            glVertexAttribPointer(layout, input.type->gl_size, (GLenum)input.type->gl_primitive_type, GL_FALSE, sizeof(Mesh::Vertex), input.offset);
        }
        for (uint k = 0; k < vertex_buffer_container->mesh_containers.entries.length(); k++)
        {
            auto mesh_container = vertex_buffer_container->mesh_containers.entries[k]->value;
            
            uint rendered_instance_count = 0;
            while (rendered_instance_count < mesh_container->active_instances_.length())
            {
                const auto instance_count = shader_->get_instance_count() == 0 ? mesh_container->active_instances_.length() : std::min(mesh_container->active_instances_.length() - rendered_instance_count, shader_->get_instance_count());

                // Instance parameters
                for (uint i = 0; i < shader_->get_instance_uniforms().length(); i++)
                {
                    const auto& param = shader_->get_instance_uniforms()[i];
                    void* parameter_buffer = malloc(param.type->c_size * instance_count);
                    for (uint j = 0; j < instance_count; j++)
                    {
                        mesh_container->active_instances_[j + rendered_instance_count]->instance_parameters_[i]->write_data((byte*)parameter_buffer + j * param.type->c_size);
                    }
                    apply_uniform(param.type->gl_type, param.layout, instance_count, parameter_buffer);
                    free(parameter_buffer);
                }

                if (shader_->get_empty_vertex())
                {
                    glDrawArraysInstanced(GL_TRIANGLES, 0, shader_->get_empty_vertex(), instance_count);
                }
                else
                {
                    glDrawArraysInstanced(GL_TRIANGLES, mesh_container->position_in_buffer, mesh_container->size_in_buffer, instance_count);
                }
                dcc++;
                
                rendered_instance_count += instance_count;
            }
        }
    }

    if (dcc > 0)
    {
        Game::instance_->draw_call_counter_[name] += dcc;
    }
}

void Material::cleanup() const
{
    //mesh_instances_->cleanup();
}

Shared<MaterialInstance> Material::create_instance()
{
    if (!Check(shader_ != nullptr, "Material", "Attempt to create instance of material with no shader!")) return nullptr;
    
    Shared<MaterialInstance> instance = create_instance_object();
    instance->material_ = weak_from_this();
    for (auto& uniform_parameter : shader_->get_instance_uniforms())
    {
        auto parameter = uniform_parameter.type->parameter_producer(uniform_parameter.name);
        parameter->reset();
        instance->instance_parameters_.add(parameter);
    }
    instance->register_direct_parameters();
    if (shader_->get_empty_vertex() > 0)
    {
        vertex_buffers_[0]->mesh_containers[nullptr]->add_instance(instance);
    }
    else
    {
        empty_instances_.add(instance);
    }
    return instance;
}

void Material::destroy_instance(const Shared<MaterialInstance>& instance)
{
    if (instance->get_material().get() == this)
    {
        if (instance->mesh_ == nullptr && shader_->get_empty_vertex() == 0)
        {
            empty_instances_.remove(instance);
        }
        else
        {
            if (const auto found = mesh_buffer_map_.find(instance->mesh_))
            {
                auto vertex_buffer_container = vertex_buffers_[*found];
                auto mesh_container = vertex_buffer_container->mesh_containers[instance->mesh_];
                
                mesh_container->remove_instance(instance);
                if (mesh_container->count() == 0 && shader_->get_empty_vertex() == 0)
                {
                    vertex_buffer_container->remove_mesh(instance->mesh_);
                    mesh_buffer_map_.remove(instance->mesh_);
                }
            }
        }
        
        instance->material_ = null_weak(Material);
    }
}

void Material::register_direct_parameters()
{
}

void Material::apply_params(const RenderData& render_data) const
{
}

Shared<MaterialInstance> Material::create_instance_object()
{
    return MakeShared<MaterialInstance>();
}

void Material::change_mesh(const Shared<MaterialInstance>& instance, const Shared<Mesh>& old_mesh, const Shared<Mesh>& new_mesh)
{
    if (instance->get_material().get() == this && old_mesh != new_mesh)
    {
        if (old_mesh == nullptr)
        {
            empty_instances_.remove(instance);
            if (const auto found = mesh_buffer_map_.find(new_mesh)) // case - no mesh -> registered mesh
            {
                vertex_buffers_[*found]->mesh_containers[new_mesh]->add_instance(instance);
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
                    use_buffer_index = vertex_buffers_.length();
                    vertex_buffers_.add(vbc);
                }
                mesh_buffer_map_[new_mesh] = use_buffer_index;
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
            empty_instances_.add(instance);
        }
        else
        {
            uint q = 0;
        }
    }
}

void Material::change_active(const Shared<MaterialInstance>& instance)
{
    if (instance->get_material().get() == this)
    {
        if (const auto found = mesh_buffer_map_.find(instance->get_mesh()))
        {
            vertex_buffers_[*found]->mesh_containers[instance->get_mesh()]->state_changed(instance);
        }
    }
}
