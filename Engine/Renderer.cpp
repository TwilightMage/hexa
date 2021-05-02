#include "Renderer.h"

#include <glad/glad.h>


#include "Game.h"
#include "IRenderable.h"
#include "Mesh.h"
#include "Shader.h"

FORCEINLINE glm::mat4 get_model_matrix(const Shared<IRenderable>& renderable)
{
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, copy_as<glm::vec3>(renderable->get_position()));

    model = rotate(model, renderable->get_rotation().axis_angle(), copy_as<glm::vec3>(renderable->get_rotation().axis()));
            				
    model = scale(model, copy_as<glm::vec3>(renderable->get_scale()));

    return model;
}

template<typename T>
FORCEINLINE T* extract_gl_buffer(uint buffer, uint buffer_type, int& buffer_size, int add_size = 0)
{
    glBindBuffer(buffer_type, buffer);
    
    glGetBufferParameteriv(buffer_type, GL_BUFFER_SIZE, &buffer_size);
    
    buffer_size /= sizeof(T);
    buffer_size += add_size;

    T* buffer_data = new T[buffer_size];

    void* buf_ptr = glMapBuffer(buffer_type, GL_READ_ONLY);
    memcpy(buffer_data, buf_ptr, sizeof(T) * buffer_size);
    glUnmapBuffer(buffer_type);

    glBindBuffer(buffer_type, 0);
    
    return buffer_data;
}

template<typename T>
FORCEINLINE void update_gl_buffer(uint buffer, uint buffer_type, T* buffer_data, int buffer_size)
{
    void* buf_ptr = glMapBuffer(buffer_type, GL_WRITE_ONLY);
    memcpy(buf_ptr, buffer_data, sizeof(T) * buffer_size);
    glUnmapBuffer(buffer_type);
}

Renderer::render_list::render_list()
    : vertex_buffer_offset(0)
    , size_in_vertex_buffer(0)
{
}

Renderer::render_list::render_list(Mesh* mesh, uint vertex_offset)
    : vertex_buffer_offset(vertex_offset)
    , size_in_vertex_buffer(mesh->get_vertices().Length())
{
}

Renderer::shader_render_data::shader_render_data()
    : gl_shader_id(-1)
    , gl_vertex_buffer_id(-1)
{
}

Renderer::shader_render_data::shader_render_data(Shader* shader)
    : gl_shader_id(shader->get_program())
{
    glGenBuffers(1, &gl_vertex_buffer_id);
}

void Renderer::shader_render_data::cleanup() const
{
    glDeleteBuffers(1, new uint[1] { gl_vertex_buffer_id });
}

void Renderer::register_object(const Weak<IRenderable>& renderable)
{
    if (const auto renderable_ptr = renderable.lock())
    {
        if (const auto shader_ptr = renderable_ptr->get_shader().lock())
        {
            if (const auto mesh_ptr = renderable_ptr->get_mesh().lock())
            {
                if (mesh_ptr->get_vertices().Length() > 0)
                {
                    if (!database.have_key(shader_ptr))
                    {
                        database[shader_ptr] = shader_render_data(shader_ptr.get());
                    }
                    auto& shader_meshes = database[shader_ptr];

                    if (!shader_meshes.have_key(mesh_ptr)) // if we adding mesh
                    {
                        // extract current buffer
                        Mesh::vertex* vertex_buffer;
                        int vertex_buffer_size;
                        if (shader_meshes.size() > 0) // if new mesh will be first
                        {
                            // extract existing vertex buffer
                            vertex_buffer = extract_gl_buffer<Mesh::vertex>(shader_meshes.gl_vertex_buffer_id, GL_ARRAY_BUFFER, vertex_buffer_size, mesh_ptr->get_vertices().Length());
                        }
                        else
                        {
                            vertex_buffer_size = mesh_ptr->get_vertices().Length();
                            // initialize new empty buffer
                            vertex_buffer = new Mesh::vertex[vertex_buffer_size];
                        }

                        // modify copy of current buffer
                        memcpy(vertex_buffer + vertex_buffer_size - mesh_ptr->get_vertices().Length(), mesh_ptr->get_vertices().GetData(), sizeof(Mesh::vertex) * mesh_ptr->get_vertices().Length());

                        glBindBuffer(GL_ARRAY_BUFFER, shader_meshes.gl_vertex_buffer_id);
                        // save updated buffer
                        glBufferData(GL_ARRAY_BUFFER, sizeof(Mesh::vertex) * vertex_buffer_size, vertex_buffer, GL_STATIC_DRAW);
                        glBindBuffer(GL_ARRAY_BUFFER, 0);
                
                        delete vertex_buffer;
                
                        shader_meshes[mesh_ptr] = render_list(mesh_ptr.get(), vertex_buffer_size - mesh_ptr->get_vertices().Length());
                    }

                    auto& mesh_objects = shader_meshes[mesh_ptr];
                    mesh_objects.Add(renderable_ptr);

                    if (mesh_objects.Length() >= render_list::objects_count_limit)
                    {
                        print_warning("Renderer", "Objects count exceeded for mesh %p for shader %s", mesh_ptr.get(), shader_ptr->get_name().c());
                    }
                }
            }
        }
    }
}

void Renderer::unregister_object(const Weak<IRenderable>& renderable)
{
    if (const auto renderable_ptr = renderable.lock())
    {
        if (const auto shader_ptr = renderable_ptr->get_shader().lock())
        {
            if (const auto mesh_ptr = renderable_ptr->get_mesh().lock())
            {
                if (database.have_key(shader_ptr))
                {
                    auto& shader_meshes = database[shader_ptr];

                    auto& entry = shader_meshes.get_entry(mesh_ptr);
                    entry.value.Remove(renderable_ptr);
                    if (entry.value.Length() == 0)
                    {
                        int vertex_buffer_size;
                        Mesh::vertex* vertex_buffer = extract_gl_buffer<Mesh::vertex>(shader_meshes.gl_vertex_buffer_id, GL_ARRAY_BUFFER, vertex_buffer_size);

                        memcpy(vertex_buffer + entry.value.vertex_buffer_offset, vertex_buffer + entry.value.vertex_buffer_offset + entry.value.size_in_vertex_buffer, sizeof(Mesh::vertex) * (vertex_buffer_size - entry.value.vertex_buffer_offset - entry.value.size_in_vertex_buffer));

                        glBindBuffer(GL_ARRAY_BUFFER, shader_meshes.gl_vertex_buffer_id);
                        // save updated buffer
                        glBufferData(GL_ARRAY_BUFFER, sizeof(Mesh::vertex) * vertex_buffer_size - entry.value.size_in_vertex_buffer, vertex_buffer, GL_STATIC_DRAW);
                        glBindBuffer(GL_ARRAY_BUFFER, 0);

                        delete vertex_buffer;

                        shader_meshes.remove_key(mesh_ptr);
                    }
                    
                    if (shader_meshes.size() == 0)
                    {
                        shader_meshes.cleanup();
                        database.remove_key(shader_ptr);
                    }
                }
            }
        }
    }
}

void Renderer::render(const glm::mat4& view_projection_matrix)
{
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glEnable(GL_CULL_FACE);
    
    for (const auto& shader_meshes : database)
    {
        glUseProgram(shader_meshes.value.gl_shader_id);
        glBindBuffer(GL_ARRAY_BUFFER, shader_meshes.value.gl_vertex_buffer_id);
        shader_meshes.key->map_params();

        for (const auto& mesh_objects : shader_meshes.value)
        {
            const uint mvp_array_size = std::min(mesh_objects.value.Length(), render_list::objects_count_limit);
            glm::mat4* mvp_array = new glm::mat4[mvp_array_size];
            {
                uint i = 0;
                for (const auto& object : mesh_objects.value)
                {
                    mvp_array[i++] = view_projection_matrix * get_model_matrix(object);
                    if (i == mvp_array_size) break;
                }
            }
            glUniformMatrix4fv(0, mvp_array_size, GL_FALSE, reinterpret_cast<float*>(mvp_array));
            delete mvp_array;
            		
            glDrawArraysInstanced(GL_TRIANGLES, mesh_objects.value.vertex_buffer_offset, mesh_objects.value.size_in_vertex_buffer, mesh_objects.value.Length());
        }
    }
}
