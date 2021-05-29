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
    
    struct Entry
    {
        FORCEINLINE Vector2 get_scale() const;
        FORCEINLINE Vector2 get_offset() const;
        
        Rect rect;
        TextureAtlas* owner;
    };

    struct UVMod
    {
        Vector2 scale;
        Vector2 offset;
    };

    uint put(const Path& path);
    uint claim_rect(const Rect& rect);
    FORCEINLINE uint get_num_entries() const;
    FORCEINLINE List<UVMod> get_cached_mods() const;
    FORCEINLINE const Entry* get_entry(uint index) const;
    FORCEINLINE void dump(const Path& path);
    FORCEINLINE Shared<Texture> to_texture() const;

    void bind(uint storage_slot) const;

    FORCEINLINE uint get_gl_texture_id() override;

    static Shared<TextureAtlas> load_png(const Path& path);
    
private:
    void generate_buffers();
    void cleanup();
    
    int size_ = 1;
    uint gl_texture_binding_ = 0;
    List<Color> pixels_ = List<Color>(1);
    List<Entry> entries_;
    List<UVMod> cached_uv_mods_;
    uint gl_mods_storage_binding_ = 0;
    int cell_width_;
    int cell_height_;

    static int max_size_;
};
