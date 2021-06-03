#pragma once

#include "IRenderable.h"
#include "Mesh.h"
#include "Shader.h"

/* STRUCTURE EXAMPLE
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

template<typename T>
FORCEINLINE T* extract_gl_buffer(uint buffer, uint buffer_type, int& buffer_size, int add_size = 0)
{
    glBindBuffer(buffer_type, buffer);
    
    glGetBufferParameteriv(buffer_type, GL_BUFFER_SIZE, &buffer_size);
    
    buffer_size /= sizeof(T);
    buffer_size += add_size;

    T* buffer_data = new T[buffer_size];

    void* buf_ptr = glMapBuffer(buffer_type, GL_READ_ONLY);
    memcpy(buffer_data, buf_ptr, sizeof(T) * (buffer_size - add_size));
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
        return entries[entries.length() - 1].value;
    }

    void clear()
    {
        entries.Clear();
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
        return entries.length();
    }

    void remove_key(const K& key)
    {
        for (uint i = 0; i < entries.length(); i++)
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
        return entries[entries.length() - 1];
    }

    entry* try_get_entry(const K& key)
    {
        for (auto& entry : entries)
        {
            if (entry.key == key) return &entry;
        }
        return nullptr;
    }

    entry& last()
    {
        return entries[entries.length() - 1];
    }
};

template<class T>
struct render_list : List<Shared<T>> // objects
{
    render_list()
        : vertex_buffer_offset(0)
        , size_in_vertex_buffer(0)
    {
    }
    
    render_list(Mesh* mesh, uint vertex_offset)
        : vertex_buffer_offset(vertex_offset)
        , size_in_vertex_buffer(mesh->get_vertices().length())
    {
    }
    
    uint vertex_buffer_offset;
    uint size_in_vertex_buffer;
};

template<class T>
struct shader_render_data : simple_map<Shared<Mesh>, render_list<T>> // objects for mesh
{
    shader_render_data()
        : gl_shader_id(-1)
        , gl_vertex_buffer_id(-1)
    {
    }
    
    explicit shader_render_data(Shader* shader)
        : gl_shader_id(shader->get_program())
    {
        glGenBuffers(1, &gl_vertex_buffer_id);
    }

    void cleanup() const
    {
        glDeleteBuffers(1, new uint[1] { gl_vertex_buffer_id });
    }
    
    uint gl_shader_id;
    uint gl_vertex_buffer_id;
};

template<class T>
struct render_database : simple_map<Shared<Shader>, shader_render_data<T>> // meshes for shader
{
    using simple_map<Shared<Shader>, shader_render_data<T>>::clear;
    using simple_map<Shared<Shader>, shader_render_data<T>>::operator[];
    using simple_map<Shared<Shader>, shader_render_data<T>>::have_key;
    using simple_map<Shared<Shader>, shader_render_data<T>>::remove_key;
    
    bool register_object(const Weak<T>& renderable)
    {
        if (const auto renderable_ptr = renderable.lock())
        {
            if (const auto& shader_ptr = renderable_ptr->get_shader())
            {
                if (const auto& mesh_ptr = renderable_ptr->get_mesh())
                {
                    if (!have_key(shader_ptr))
                    {
                        operator[](shader_ptr) = shader_render_data<T>(shader_ptr.get());
                    }

                    return register_object_to_mesh(renderable_ptr, shader_ptr, mesh_ptr);
                }
            }
        }

        return false;
    }

    bool unregister_object(const Weak<T>& renderable)
    {
        if (const auto renderable_ptr = renderable.lock())
        {
            if (const auto shader_ptr = renderable_ptr->get_shader())
            {
                if (const auto mesh_ptr = renderable_ptr->get_mesh())
                {
                    if (have_key(shader_ptr))
                    {
                        if (unregister_object_from_mesh(renderable_ptr, shader_ptr, mesh_ptr))
                        {
                            auto& shader_meshes = operator[](shader_ptr);
                            if (shader_meshes.size() == 0)
                            {
                                shader_meshes.cleanup();
                                remove_key(shader_ptr);
                            }

                            return true;
                        }
                    }
                }
            }
        }

        return false;
    }

    std::map<Texture*, uint> dump_texture_usage() const
    {
        std::map<Texture*, uint> result;
        
        for (const auto& shader_mesh : *this)
        {
            for (const auto& mesh_objects : shader_mesh.value)
            {
                for (const auto& object : mesh_objects.value)
                {
                    result[object->get_texture().get()]++;
                }
            }
        }

        return result;
    }

    void cleanup()
    {
        for (auto& shader_meshes : *this)
        {
            shader_meshes.value.cleanup();
        }
        clear();
    }

    bool change_object_mesh(const Weak<T> renderable, const Weak<Mesh>& old_mesh)
    {
        if (const auto renderable_ptr = renderable.lock())
        {
            if (const auto shader_ptr = renderable_ptr->get_shader())
            {
                if (const auto mesh_ptr = renderable_ptr->get_mesh())
                {
                    if (const auto old_mesh_ptr = old_mesh.lock())
                    {
                        if (have_key(shader_ptr))
                        {
                            if (mesh_ptr != old_mesh_ptr)
                            {
                                return unregister_object_from_mesh(renderable_ptr, shader_ptr, old_mesh_ptr) && register_object_to_mesh(renderable_ptr, shader_ptr, mesh_ptr);
                            }
                        }
                    }
                }
            }
        }

        return false;
    }

    bool change_object_shader(const Weak<T> renderable, const Weak<Shader> old_shader)
    {
        if (const auto renderable_ptr = renderable.lock())
        {
            if (const auto shader_ptr = renderable_ptr->get_shader())
            {
                if (const auto mesh_ptr = renderable_ptr->get_mesh())
                {
                    if (have_key(shader_ptr))
                    {
                        unregister_object_from_mesh(renderable_ptr, shader_ptr, mesh_ptr);
                        
                        auto& shader_meshes = operator[](shader_ptr);
                        if (shader_meshes.size() == 0)
                        {
                            shader_meshes.cleanup();
                            remove_key(shader_ptr);
                        }
                    }

                    if (!have_key(shader_ptr))
                    {
                        operator[](shader_ptr) = shader_render_data<T>(shader_ptr.get());
                    }

                    return register_object_to_mesh(renderable_ptr, shader_ptr, mesh_ptr);
                }
            }
        }

        return false;
    }

private:
    bool register_object_to_mesh(const Shared<T>& renderable_ptr, const Shared<Shader>& shader_ptr, const Shared<Mesh>& mesh_ptr)
    {
        auto& shader_meshes = operator[](shader_ptr);
        
        if (!shader_meshes.have_key(mesh_ptr)) // if we adding mesh
        {
            // extract current buffer
            Mesh::Vertex* vertex_buffer;
            int vertex_buffer_size;
            if (shader_meshes.size() > 0) // if new mesh will be first
            {
                // extract existing vertex buffer
                vertex_buffer = extract_gl_buffer<Mesh::Vertex>(shader_meshes.gl_vertex_buffer_id, GL_ARRAY_BUFFER, vertex_buffer_size, mesh_ptr->get_vertices().length());
            }
            else
            {
                vertex_buffer_size = mesh_ptr->get_vertices().length();
                // initialize new empty buffer
                vertex_buffer = new Mesh::Vertex[vertex_buffer_size];
            }

            // modify copy of current buffer
            memcpy(vertex_buffer + vertex_buffer_size - mesh_ptr->get_vertices().length(), mesh_ptr->get_vertices().get_data(), sizeof(Mesh::Vertex) * mesh_ptr->get_vertices().length());

            glBindBuffer(GL_ARRAY_BUFFER, shader_meshes.gl_vertex_buffer_id);
            // save updated buffer
            glBufferData(GL_ARRAY_BUFFER, sizeof(Mesh::Vertex) * vertex_buffer_size, vertex_buffer, GL_STATIC_DRAW);
            glBindBuffer(GL_ARRAY_BUFFER, 0);
    
            delete vertex_buffer;
    
            shader_meshes[mesh_ptr] = render_list<T>(mesh_ptr.get(), vertex_buffer_size - mesh_ptr->get_vertices().length());
        }

        auto& mesh_objects = shader_meshes[mesh_ptr];

        if (mesh_objects.Contains(renderable_ptr)) return false;

        mesh_objects.Add(renderable_ptr);
        return true;
    }

    bool unregister_object_from_mesh(const Shared<T>& renderable_ptr, const Shared<Shader>& shader_ptr, const Shared<Mesh>& mesh_ptr)
    {
        auto& shader_meshes = operator[](shader_ptr);

        if (auto mesh_objects = shader_meshes.try_get_entry(mesh_ptr))
        {
            const auto index = mesh_objects->value.IndexOf(renderable_ptr);
            if (index > -1)
            {
                mesh_objects->value.RemoveAt(index);
                if (mesh_objects->value.length() == 0)
                {
                    int vertex_buffer_size;
                    Mesh::Vertex* vertex_buffer = extract_gl_buffer<Mesh::Vertex>(shader_meshes.gl_vertex_buffer_id, GL_ARRAY_BUFFER, vertex_buffer_size);

                    memcpy(vertex_buffer + mesh_objects->value.vertex_buffer_offset, vertex_buffer + mesh_objects->value.vertex_buffer_offset + mesh_objects->value.size_in_vertex_buffer, sizeof(Mesh::Vertex) * (vertex_buffer_size - mesh_objects->value.vertex_buffer_offset - mesh_objects->value.size_in_vertex_buffer));
                    for (auto& entry : shader_meshes)
                    {
                        if (entry.value.vertex_buffer_offset > mesh_objects->value.vertex_buffer_offset)
                        {
                            entry.value.vertex_buffer_offset -= mesh_objects->value.size_in_vertex_buffer;
                        }
                    }
                    
                    glBindBuffer(GL_ARRAY_BUFFER, shader_meshes.gl_vertex_buffer_id);
                    // save updated buffer
                    glBufferData(GL_ARRAY_BUFFER, sizeof(Mesh::Vertex) * vertex_buffer_size - mesh_objects->value.size_in_vertex_buffer, vertex_buffer, GL_STATIC_DRAW);
                    glBindBuffer(GL_ARRAY_BUFFER, 0);

                    delete vertex_buffer;

                    shader_meshes.remove_key(mesh_ptr);
                }

                return true;
            }
        }

        return false;
    }
};
