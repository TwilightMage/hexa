#pragma once

#include "BasicTypes.h"
#include "Color.h"
#include "framework.h"
#include "List.h"
#include "Object.h"
#include "Path.h"
#include "Rect.h"
#include "Texture.h"
#include "Vector2.h"

class Game;

EXTERN class EXPORT TextureAtlas : public Object, public ITexture
{
    friend Game;

public:
    TextureAtlas(const String& name);
    TextureAtlas(const String& name, int cell_width, int cell_height);
    
    struct entry
    {
        Vector2 get_scale() const;
        Vector2 get_offset() const;
        
        Rect rect;
        TextureAtlas* owner;
    };

    struct uv_mod
    {
        Vector2 scale;
        Vector2 offset;
    };

    uint put(const Path& path);
    uint get_num_entries() const;
    List<uv_mod> get_cached_mods() const;
    const entry* get_entry(uint index) const;
    void dump(const Path& path);
    Shared<Texture> to_texture() const;

    void bind(uint storage_slot) const;

    virtual uint get_gl_texture_id() override;
    
private:
    void generate_buffers();
    void cleanup();
    
    int size_ = 1;
    uint gl_texture_binding_ = 0;
    List<Color> pixels_ = List<Color>(1);
    List<entry> entries_;
    List<uv_mod> cached_uv_mods_;
    uint gl_mods_storage_binding_ = 0;
    int cell_width_;
    int cell_height_;

    static int max_size_;
    static bool is_loading_stage_;
    static bool is_render_stage_;
};
