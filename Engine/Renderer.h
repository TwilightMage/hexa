#pragma once
#include <glm/fwd.hpp>


#include "List.h"

class Mesh;
class Shader;
class IRenderable;

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

class Renderer
{
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

public:
    void register_object(const Weak<IRenderable>& renderable);
    void unregister_object(const Weak<IRenderable>& renderable);

    void render(const glm::mat4& view_projection_matrix);

private:
    render_database database;
};
