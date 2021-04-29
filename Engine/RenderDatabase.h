#pragma once

#include <map>
#include <glad/glad.h>

#include "IRenderable.h"
#include "List.h"
#include "Shader.h"
#include "Utils.h"

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

__forceinline glm::mat4 get_model_matrix(class IRenderable* renderable)
{
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, renderable->get_position());

    const auto& rot = renderable->get_rotation();
    const auto rot_angle = 2 * acos(rot.w);
    const auto rot_axis = rot.w == 1.0f ? glm::vec3(0.0f, 0.0f, 1.0f) : glm::vec3(rot.x / sqrt(1 - rot.w * rot.w), rot.y / sqrt(1 - rot.w * rot.w), rot.z / sqrt(1 - rot.w * rot.w));
    model = rotate(model, rot_angle, rot_axis);
            				
    model = scale(model, renderable->get_scale());

    return model;
}

template<typename T>
__forceinline T* extract_gl_buffer(uint buffer, uint buffer_type, int& buffer_size, int add_size = 0)
{
    glGetBufferParameteriv(buffer_type, GL_BUFFER_SIZE, &buffer_size);
    
    buffer_size /= sizeof(T);
    buffer_size += add_size;

    T* buffer_data = new T[buffer_size];

    void* buf_ptr = glMapBuffer(buffer_type, GL_READ_ONLY);
    memcpy(buffer_data, buf_ptr, sizeof(T) * buffer_size);
    glUnmapBuffer(buffer_type);

    return buffer_data;
}

template<typename T>
__forceinline void update_gl_buffer(uint buffer, uint buffer_type, T* buffer_data, int buffer_size)
{
    void* buf_ptr = glMapBuffer(buffer_type, GL_WRITE_ONLY);
    memcpy(buf_ptr, buffer_data, sizeof(T) * buffer_size);
    glUnmapBuffer(buffer_type);
}

template<typename K, typename V>
struct simple_map
{
    struct entry
    {
        K key;
        V value;

        bool operator==(const entry& rhs) const
        {
            return key == rhs.key = value == rhs.value;
        }
    };
    List<entry> entries;

    V& operator[](const K& key)
    {
        for (auto& entry : entries)
        {
            if (entry.key == key) return entry.value;
        }
        entries.Add({ key, V() });
        return entries[entries.Length() - 1].value;
    }

    entry* begin()
    {
        return entries.begin();
    }

    entry* end()
    {
        return entries.end();
    }

    const entry* begin() const
    {
        return entries.begin();
    }

    const entry* end() const
    {
        return entries.end();
    }

    uint size()
    {
        return entries.Length();
    }

    bool have_key(const K& key)
    {
        for (auto& kvp : entries)
        {
            if (kvp.key == key) return true;
        }

        return false;
    }

    entry& last()
    {
        return entries[entries.Length() - 1];
    }
};

struct render_list : List<Shared<class IRenderable>> // objects
{
    render_list()
        : vertex_buffer_offset(0)
        , size_in_vertex_buffer(0)
    {
    }

    render_list(class Mesh* mesh, uint vertex_offset)
    {
        vertex_buffer_offset = vertex_offset;
        size_in_vertex_buffer = mesh->vertices.Length();
    }
    
    uint vertex_buffer_offset;
    uint size_in_vertex_buffer;
};

struct shader_render_data : simple_map<Shared<class Mesh>, render_list> // objects for mesh
{
    shader_render_data()
        : gl_shader_id(-1)
        , gl_vertex_buffer_id(-1)
        , objects_count(0)
    {
    }

    explicit shader_render_data(class Shader* shader)
    {
        gl_shader_id = shader->get_program();
        glGenBuffers(1, &gl_vertex_buffer_id);
        objects_count = 0;
    }
    
    void add(class IRenderable* renderable)
    {
        const Shared<Mesh> mesh = Shared<Mesh>(renderable->get_mesh()); // We assume that mesh is valid since we checked it earlier
                   
        if (!have_key(mesh)) // if we adding mesh
        {
            // extract current buffer
            Mesh::vertex* vertex_buffer;
            int vertex_buffer_size;
            if (size() > 0) // if new mesh will be first
            {
                // extract existing vertex buffer
                vertex_buffer = extract_gl_buffer<Mesh::vertex>(GL_ARRAY_BUFFER, gl_vertex_buffer_id, vertex_buffer_size, mesh->vertices.Length());
            }
            else
            {
                vertex_buffer_size = mesh->vertices.Length();
                // initialize new empty buffer
                vertex_buffer = new Mesh::vertex[vertex_buffer_size];
            }

            // modify copy of current buffer
            memcpy(vertex_buffer + vertex_buffer_size - mesh->vertices.Length(), mesh->vertices.GetData(), sizeof(Mesh::vertex) * mesh->vertices.Length());

            glBindBuffer(GL_ARRAY_BUFFER, gl_vertex_buffer_id);
            // save updated buffer
            glBufferData(GL_ARRAY_BUFFER, sizeof(Mesh::vertex) * vertex_buffer_size, vertex_buffer, GL_STATIC_DRAW);
            glBindBuffer(GL_ARRAY_BUFFER, 0);
            
#if ENABLE_DEBUG_RENDER
            cached_vertices = List<Mesh::vertex>(vertex_buffer, vertex_buffer_size);
#endif
            delete vertex_buffer;
            
            operator[](mesh) = render_list(mesh.get(), vertex_buffer_size - mesh->vertices.Length());
        }
        
        operator[](mesh).Add(Shared<IRenderable>(renderable));
        objects_count++;
    }

    void cleanup() const
    {
        glDeleteBuffers(1, new uint[1] { gl_vertex_buffer_id });
    }
    
    uint gl_shader_id;
    uint gl_vertex_buffer_id;
    uint objects_count;

#if ENABLE_DEBUG_RENDER
    List<Mesh::vertex> cached_vertices;
#endif
};

struct render_database : simple_map<Shared<class Shader>, shader_render_data> // meshes for shader
{
    void add(class IRenderable* renderable)
    {
        if (const auto shared_ptr = renderable->get_shader().lock())
        {
            if (const auto mesh_ptr = renderable->get_mesh().lock())
            {
                if (mesh_ptr->vertices.Length() > 0)
                {
                    if (!have_key(shared_ptr))
                    {
                        operator[](shared_ptr) = shader_render_data(shared_ptr.get());
                    }
                    operator[](shared_ptr).add(renderable);
                }
            }
        }
    }
};