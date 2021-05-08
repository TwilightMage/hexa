#pragma once

#include <map>

#include "Color.h"
#include "Entity.h"
#include "ITexture.h"
#include "List.h"
#include "Object.h"
#include "Path.h"

class TextureSlot;
class UIElement;
class Image;
class Game;

EXTERN class EXPORT Texture : public Object, public ITexture
{
    friend Game;
    friend TextureSlot;
    
public:
    Texture(const String& name);
    Texture(const String& name, uint width, uint height, List<Color> pixels);
    
    static Shared<Texture> load_png(const Path& path);

    uint get_gl_texture_id() override;
    uint64 get_handle_arb() const;
    uint get_width() const;
    uint get_height() const;

    static const std::map<Texture*, uint>& get_usage_counter();
    
private:
    void usage_count_increase();
    void usage_count_decrease();
    void usage_count_changed();
    void cleanup();
    
    List<Color> pixels_;
    uint width_ = 0;
    uint height_ = 0;
    uint gl_texture_binding_ = 0;
    uint64 handle_arb_ = 0;

    static std::map<Texture*, uint> usage_counter_;
};
