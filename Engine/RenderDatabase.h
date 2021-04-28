#pragma once

#include <map>
#include <glad/glad.h>

#include "IRenderable.h"
#include "List.h"
#include "Shader.h"
#include "Utils.h"

#define DEBUG_RENDER 1

/* EXAMPLE
*
* Game
* |
* +--basic_shader
* |  |
* |  +--player_model
* |  |  |
* |  |  +--player_1
* |  |  |
* |  |  +--player_2
* |  |
* |  +--wolf_model
* |     |
* |     +--wolf_1
* |     |
* |     +--wolf_2
* |     |
* |     +--wolf_3
* |
* +--transparent_shader
*    |
*    +--bottle_model
*       |
*       +--bottle_1
*/

struct render_list : List<Shared<class IRenderable>> // objects
{
    render_list()
        : vertex_buffer_offset(0)
        , size_in_vertex_buffer(0)
    {
    }

    render_list(class Mesh* mesh, uint offset)
    {
        vertex_buffer_offset = offset;
        size_in_vertex_buffer = mesh->vertices.Length();
    }
    
    uint vertex_buffer_offset;
    uint size_in_vertex_buffer;
};

struct shader_render_data : std::map<Shared<class Mesh>, render_list> // objects for mesh
{
    shader_render_data()
        : gl_shader_id(0)
        , gl_vertex_buffer_id(0)
    {
    }

    explicit shader_render_data(class Shader* shader)
    {
        gl_shader_id = shader->get_program();
        glGenBuffers(1, &gl_vertex_buffer_id);
    }
    
    void add(class IRenderable* renderable)
    {
        const Shared<Mesh> mesh = Shared<Mesh>(renderable->get_mesh()); // We assume that mesh is valid since we checked it earlier
                   
        if (find(mesh) == end()) // if we adding mesh
        {
            glBindBuffer(GL_ARRAY_BUFFER, gl_vertex_buffer_id);

            // extract current buffer
            Mesh::vertex* vertex_buffer;
            int copy_data_to_offset = 0;
            if (size() > 0) // if new mesh will be first
            {
                // get current buffer size
                glGetBufferParameteriv(GL_ARRAY_BUFFER, GL_BUFFER_SIZE, &copy_data_to_offset);
                copy_data_to_offset /= sizeof(Mesh::vertex);

                // initialize new buffer
                vertex_buffer = new Mesh::vertex[copy_data_to_offset + mesh->vertices.Length()];

                // read current buffer into current
                void* buf_ptr = glMapBuffer(GL_ARRAY_BUFFER, GL_READ_ONLY);
                memcpy(vertex_buffer, buf_ptr, sizeof(Mesh::vertex) * copy_data_to_offset);
                glUnmapBuffer(GL_ARRAY_BUFFER);
            }
            else
            {
                // initialize new empty buffer
                vertex_buffer = new Mesh::vertex[mesh->vertices.Length()];
            }

            // modify copy of current buffer
            memcpy(vertex_buffer + copy_data_to_offset, mesh->vertices.GetData(), sizeof(Mesh::vertex) * mesh->vertices.Length());

            // save updated buffer
            glBufferData(GL_ARRAY_BUFFER, sizeof(Mesh::vertex) * (copy_data_to_offset + mesh->vertices.Length()), vertex_buffer, GL_STATIC_DRAW);
        
            glBindBuffer(GL_ARRAY_BUFFER, 0);
#if DEBUG_RENDER
            cached_vertices = List<Mesh::vertex>(vertex_buffer, copy_data_to_offset + mesh->vertices.Length());
#endif
            delete vertex_buffer;
                
            operator[](mesh) = render_list(mesh.get(), copy_data_to_offset);
        }
        operator[](mesh).Add(Shared<IRenderable>(renderable));
    }

    void cleanup() const
    {
        glDeleteBuffers(1, new uint[1] { gl_vertex_buffer_id });
    }
    
    uint gl_shader_id;
    uint gl_vertex_buffer_id;

#if DEBUG_RENDER
    List<Mesh::vertex> cached_vertices;
#endif
};

struct render_database : std::map<Shared<class Shader>, shader_render_data> // meshes for shader
{
    void add(class IRenderable* renderable)
    {
        const Shared<Shader> shader = Shared<Shader>(renderable->get_shader());
        Mesh* mesh = renderable->get_mesh();
        if (shader && mesh && mesh->vertices.Length() > 0)
        {
            if (find(shader) == end())
            {
                operator[](shader) = shader_render_data(shader.get());
            }
            operator[](shader).add(renderable);
        }
    }
};

template<typename T>
List<T> dump_gl_buffer(uint buffer, uint buffer_type)
{
    int buffer_size;
    glGetBufferParameteriv(GL_ARRAY_BUFFER, GL_BUFFER_SIZE, &buffer_size);
    buffer_size /= sizeof(T);

    T* buffer_data = new T[buffer_size];

    void* buf_ptr = glMapBuffer(GL_ARRAY_BUFFER, GL_READ_ONLY);
    memcpy(buffer_data, buf_ptr, sizeof(T) * buffer_size);
    glUnmapBuffer(GL_ARRAY_BUFFER);

    return List<T>(buffer_data, buffer_size);
}