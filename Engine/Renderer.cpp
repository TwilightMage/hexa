#include "Renderer.h"

#include <glad/glad.h>


#include "Game.h"
#include "IRenderable.h"
#include "Mesh.h"
#include "Shader.h"
#include "TextureAtlas.h"

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
    render_list();
    render_list(Mesh* mesh, uint vertex_offset);

    inline static const uint objects_count_limit = 256;
    
    uint vertex_buffer_offset;
    uint size_in_vertex_buffer;
};

struct shader_render_data : simple_map<Shared<Mesh>, render_list> // objects for mesh
{
    shader_render_data();
    explicit shader_render_data(Shader* shader);

    void cleanup() const;
    
    uint gl_shader_id;
    uint gl_vertex_buffer_id;
};

struct render_database : simple_map<Shared<Shader>, shader_render_data> // meshes for shader
{
};

FORCEINLINE glm::mat4 get_model_matrix(const Shared<IRenderable>& renderable)
{
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, cast_object<glm::vec3>(renderable->get_position()));

    model = rotate(model, renderable->get_rotation().axis_angle(), cast_object<glm::vec3>(renderable->get_rotation().axis()));
            				
    model = scale(model, cast_object<glm::vec3>(renderable->get_scale()));

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

render_list::render_list()
    : vertex_buffer_offset(0)
    , size_in_vertex_buffer(0)
{
}

render_list::render_list(Mesh* mesh, uint vertex_offset)
    : vertex_buffer_offset(vertex_offset)
    , size_in_vertex_buffer(mesh->get_vertices().Length())
{
}

shader_render_data::shader_render_data()
    : gl_shader_id(-1)
    , gl_vertex_buffer_id(-1)
{
}

shader_render_data::shader_render_data(Shader* shader)
    : gl_shader_id(shader->get_program())
{
    glGenBuffers(1, &gl_vertex_buffer_id);
}

void shader_render_data::cleanup() const
{
    glDeleteBuffers(1, new uint[1] { gl_vertex_buffer_id });
}

Renderer::Renderer()
{
    database_ = new render_database();
}

Renderer::~Renderer()
{
    delete database_;
}

void Renderer::register_object(const Weak<IRenderable>& renderable) const
{
    auto& db = *database_;
    if (const auto renderable_ptr = renderable.lock())
    {
        if (const auto shader_ptr = renderable_ptr->get_shader().lock())
        {
            if (const auto mesh_ptr = renderable_ptr->get_mesh().lock())
            {
                if (mesh_ptr->get_vertices().Length() > 0)
                {
                    if (!db.have_key(shader_ptr))
                    {
                        db[shader_ptr] = shader_render_data(shader_ptr.get());
                    }
                    auto& shader_meshes = db[shader_ptr];

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
                }
            }
        }
    }
}

void Renderer::unregister_object(const Weak<IRenderable>& renderable) const
{
    auto& db = *database_;
    if (const auto renderable_ptr = renderable.lock())
    {
        if (const auto shader_ptr = renderable_ptr->get_shader().lock())
        {
            if (const auto mesh_ptr = renderable_ptr->get_mesh().lock())
            {
                if (db.have_key(shader_ptr))
                {
                    auto& shader_meshes = db[shader_ptr];

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
                        db.remove_key(shader_ptr);
                    }
                }
            }
        }
    }
}

void Renderer::render(const glm::mat4& view_projection_matrix, class TextureAtlas* atlas) const
{
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glEnable(GL_CULL_FACE);

    auto& db = *database_;
    for (const auto& shader_meshes : db)
    {
        glUseProgram(shader_meshes.value.gl_shader_id);
        glBindBuffer(GL_ARRAY_BUFFER, shader_meshes.value.gl_vertex_buffer_id);
        shader_meshes.key->map_params();

        glUniform4fv(460, atlas->get_num_entries(), reinterpret_cast<float*>(atlas->get_cached_mods().GetData()));

        for (const auto& mesh_objects : shader_meshes.value)
        {
            const uint instance_count = std::min(mesh_objects.value.Length(), render_list::objects_count_limit);
            glm::mat4* mvp_array = new glm::mat4[instance_count];
            int* atlas_entry_id_array = new int[instance_count];
            {
                uint i = 0;
                for (const auto& object : mesh_objects.value)
                {
                    mvp_array[i] = view_projection_matrix * get_model_matrix(object);
                    atlas_entry_id_array[i] = 5;
                    
                    if (++i == instance_count) break;
                }
            }
            glUniformMatrix4fv(0, instance_count, GL_FALSE, reinterpret_cast<float*>(mvp_array));
            glUniform1iv(230, instance_count, atlas_entry_id_array);
            delete[] mvp_array;
            delete[] atlas_entry_id_array;
            		
            glDrawArraysInstanced(GL_TRIANGLES, mesh_objects.value.vertex_buffer_offset, mesh_objects.value.size_in_vertex_buffer, mesh_objects.value.Length());
        }
    }
}

void Renderer::cleanup() const
{
    auto& db = *database_;
    for (auto& shader_meshes : db)
    {
        shader_meshes.value.cleanup();
    }
    db.clear();
}
