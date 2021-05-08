#pragma once

#include <map>

#include "Color.h"
#include "Entity.h"
#include "ITexture.h"
#include "IUsageCountable.h"
#include "List.h"
#include "Object.h"
#include "Path.h"

class TextureSlot;
class UIElement;
class Image;
class Game;

EXTERN class EXPORT Texture : public Object, public ITexture, public IUsageCountable<Texture>, public std::enable_shared_from_this<Texture>
{
    IMPLEMENT_USAGE_COUNTER(Texture);
    
public:
    class Editor;
    
private:
    friend Game;
    friend TextureSlot;
    friend Editor;
    
public:
    class Editor
    {
        friend Texture;
        
    public:
        Editor(const Editor& rhs);
        Editor& operator=(const Editor& rhs);
        ~Editor();
        
        void set_pixel(uint x, uint y, const Color& color) const;
        void process_pixels(Color(* processor)(uint x, uint y, const Color&)) const;
        
    private:
        explicit Editor(const Shared<Texture>& target);
        
        Shared<Texture> target_;
    };
    
    Texture(const String& name);
    Texture(const String& name, uint width, uint height, List<Color> pixels);
    
    static Shared<Texture> load_png(const Path& path);

    uint get_gl_texture_id() override;
    uint64 get_handle_arb() const;
    uint get_width() const;
    uint get_height() const;
    Color get_pixel(uint x, uint y) const;

    Shared<Editor> edit();

    static const std::map<Texture*, uint>& get_usage_counter();
    uint usage_count() const;
    
private:
    void usage_count_increase();
    void usage_count_decrease();
    void usage_count_changed();
    void activate();
    void edit_count_increase();
    void edit_count_decrease();
    static void cleanup();
    
    List<Color> pixels_;
    uint width_ = 0;
    uint height_ = 0;
    uint gl_texture_binding_ = 0;
    uint64 handle_arb_ = 0;

    //static std::map<Texture*, uint> usage_counter_;
    uint edit_count_;
    bool delayed_activation_;
};
