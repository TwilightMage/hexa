#pragma once

#include "BasicTypes.h"
#include "Color.h"
#include "framework.h"
#include "List.h"
#include "Path.h"
#include "Rect.h"
#include "Texture.h"
#include "Vector2.h"

class Game;

EXTERN class EXPORT TextureAtlas : public ITexture
{
    friend Game;

public:
    TextureAtlas();
    TextureAtlas(int cell_width, int cell_height);
    
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

    virtual uint get_gl_id() override;
    
private:
    void usage_count_changed();
    void cleanup();
    
    int size_ = 1;
    uint usage_count_ = 0;
    uint gl_binding_ = 0;
    List<Color> pixels_ = List<Color>(1);
    List<entry> entries_;
    List<uv_mod> cached_uv_mods;
    int cell_width_;
    int cell_height_;

    static int max_size_;
};
