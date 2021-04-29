#pragma once

#include <map>
#include <glad/glad.h>

#include "IRenderable.h"
#include "List.h"
#include "Shader.h"

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

__forceinline glm::mat4 get_model_matrix(const Shared<IRenderable>& renderable)
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

    void remove_key(const K& key)
    {
        for (uint i = 0; i < entries.Length(); i++)
        {
            if (entries[i].key == key)
            {
                entries.RemoveAt(i);
                return;
            }
        }
    }

    bool have_key(const K& key)
    {
        for (auto& kvp : entries)
        {
            if (kvp.key == key) return true;
        }

        return false;
    }

    entry& get_entry(const K& key)
    {
        for (auto& entry : entries)
        {
            if (entry.key == key) return entry;
        }
        entries.Add({ key, V() });
        return entries[entries.Length() - 1];
    }

    entry& last()
    {
        return entries[entries.Length() - 1];
    }
};

struct render_list : List<Shared<IRenderable>> // objects
{
    render_list()
        : vertex_buffer_offset(0)
        , size_in_vertex_buffer(0)
    {
    }

    render_list(Mesh* mesh, uint vertex_offset)
    {
        vertex_buffer_offset = vertex_offset;
        size_in_vertex_buffer = mesh->vertices.Length();
    }

    void add(const Shared<IRenderable>& renderable)
    {
        Add(renderable);

        if (Length() >= objects_count_limit)
        {
            // TODO: Print warning
        }
    }

    inline static const uint objects_count_limit = 256;
    
    uint vertex_buffer_offset;
    uint size_in_vertex_buffer;
};

struct shader_render_data : simple_map<Shared<Mesh>, render_list> // objects for mesh
{
    shader_render_data()
        : gl_shader_id(-1)
        , gl_vertex_buffer_id(-1)
    {
    }

    explicit shader_render_data(Shader* shader)
    {
        gl_shader_id = shader->get_program();
        glGenBuffers(1, &gl_vertex_buffer_id);
    }
    
    void add(const Shared<IRenderable>& renderable)
    {
        if (const auto mesh = renderable->get_mesh().lock())
        {
            if (!have_key(mesh)) // if we adding mesh
                {
                // extract current buffer
                Mesh::vertex* vertex_buffer;
                int vertex_buffer_size;
                if (size() > 0) // if new mesh will be first
                    {
                    // extract existing vertex buffer
                    vertex_buffer = extract_gl_buffer<Mesh::vertex>(gl_vertex_buffer_id, GL_ARRAY_BUFFER, vertex_buffer_size, mesh->vertices.Length());
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
        
            operator[](mesh).add(renderable);
        }
    }

    void remove(const Shared<IRenderable>& renderable)
    {
        if (const auto mesh = renderable->get_mesh().lock())
        {
            auto& entry = get_entry(mesh);
            entry.value.Remove(renderable);
            if (entry.value.Length() == 0)
            {
                int vertex_buffer_size;
                Mesh::vertex* vertex_buffer = extract_gl_buffer<Mesh::vertex>(gl_vertex_buffer_id, GL_ARRAY_BUFFER, vertex_buffer_size);

                memcpy(vertex_buffer + entry.value.vertex_buffer_offset, vertex_buffer + entry.value.vertex_buffer_offset + entry.value.size_in_vertex_buffer, sizeof(Mesh::vertex) * (vertex_buffer_size - entry.value.vertex_buffer_offset - entry.value.size_in_vertex_buffer));

                glBindBuffer(GL_ARRAY_BUFFER, gl_vertex_buffer_id);
                // save updated buffer
                glBufferData(GL_ARRAY_BUFFER, sizeof(Mesh::vertex) * vertex_buffer_size - entry.value.size_in_vertex_buffer, vertex_buffer, GL_STATIC_DRAW);
                glBindBuffer(GL_ARRAY_BUFFER, 0);

                delete vertex_buffer;

                remove_key(mesh);
            }
        }
    }

    void cleanup() const
    {
        glDeleteBuffers(1, new uint[1] { gl_vertex_buffer_id });
    }
    
    uint gl_shader_id;
    uint gl_vertex_buffer_id;

#if ENABLE_DEBUG_RENDER
    List<Mesh::vertex> cached_vertices;
#endif
};

struct render_database : simple_map<Shared<Shader>, shader_render_data> // meshes for shader
{
    void add(const Weak<IRenderable>& renderable)
    {
        if (auto renderable_ptr = renderable.lock())
        {
            if (const auto shader_ptr = renderable_ptr->get_shader().lock())
            {
                if (const auto mesh_ptr = renderable_ptr->get_mesh().lock())
                {
                    if (mesh_ptr->vertices.Length() > 0)
                    {
                        if (!have_key(shader_ptr))
                        {
                            operator[](shader_ptr) = shader_render_data(shader_ptr.get());
                        }
                        operator[](shader_ptr).add(renderable_ptr);
                    }
                }
            }
        }
    }

    void remove(const Weak<IRenderable>& renderable)
    {
        if (const auto renderable_ptr = renderable.lock())
        {
            if (const auto shader_ptr = renderable_ptr->get_shader().lock())
            {
                if (const auto mesh_ptr = renderable_ptr->get_mesh().lock())
                {
                    if (have_key(shader_ptr))
                    {
                        auto& shader_meshes = operator[](shader_ptr);
                        shader_meshes.remove(renderable_ptr);
                        if (shader_meshes.size() == 0)
                        {
                            shader_meshes.cleanup();
                            remove_key(shader_ptr);
                        }
                    }
                }
            }
        }
    }
};