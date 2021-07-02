#pragma once

#include <map>

#include "Array2D.h"
#include "Color.h"
#include "ITexture.h"
#include "IUsageCountable.h"
#include "List.h"
#include "Logger.h"
#include "Object.h"
#include "Path.h"
#include "Slot.h"
#include "UsageCounter.h"

class Game;
class UIElement;
class Image;

class EXPORT Texture : public Object, public ITexture, public IUsageCountable<Texture>, public std::enable_shared_from_this<Texture>
{
    IMPLEMENT_USAGE_COUNTER(Texture);
    
public:
    class Editor;
    
private:
    friend Game;
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
    Texture(const String& name, const Array2D<Color>& pixels);
    Texture(const String& name, uint width, uint height, const List<Color>& pixels);
    
    static Shared<Texture> load_png(const Path& path);

    uint get_gl_texture_id() override;
    uint64 get_handle_arb() const;
    uint get_width() const;
    uint get_height() const;
    Vector2 get_size() const;
    Color get_pixel(uint x, uint y) const;

    void save_to_file(const Path& path) override;
    
    FORCEINLINE void put_pixels(const Array2D<Color>& pixels);
    void put_pixels(uint width, uint height, const List<Color>& pixels);

    Shared<Editor> edit();

protected:
    void load() override;
    void unload() override;
    
private:
    void load_internal();
    void edit_count_increase();
    void edit_count_decrease();
    
    List<Color> pixels_;
    uint width_ = 0;
    uint height_ = 0;
    uint gl_texture_binding_ = 0;
    uint64 handle_arb_ = 0;

    //static std::map<Texture*, uint> usage_counter_;
    uint edit_count_;
    bool delayed_activation_;
};

typedef Slot<Texture> TextureSlot;
