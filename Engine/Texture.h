﻿#pragma once

#include <map>
#include <OGRE/OgreHeaderSuffix.h>

#include "Array2D.h"
#include "Color.h"
#include "ITexture.h"
#include "IUsageCountable.h"
#include "List.h"
#include "ModuleAssetID.h"
#include "Object.h"
#include "Path.h"
#include "Pointers.h"
#include "Slot.h"
#include "UsageCounter.h"

class Material;
class Module;
class Game;
class UIElement;
class Image;

namespace Ogre
{
    class Texture;
}

class EXPORT Texture : public ITexture, public IUsageCountable<Texture>, public EnableSharedFromThis<Texture>
{
    IMPLEMENT_USAGE_COUNTER(Texture);
    
public:
    class Editor;
    
private:
    friend Game;
    friend Editor;
    friend Module;
    friend Material;
    
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
    
    Texture();
    Texture(const Array2D<Color>& pixels);
    Texture(uint width, uint height, const List<Color>& pixels);
    
    static Shared<Texture> load_png(const Path& path);

    FORCEINLINE uint get_gl_texture_id() override;
    FORCEINLINE uint64 get_handle_arb() const;
    FORCEINLINE uint get_width() const;
    FORCEINLINE uint get_height() const;
    FORCEINLINE Vector2 get_size() const;
    FORCEINLINE Color get_pixel(uint x, uint y) const;

    void save_to_file(const Path& path) override;
    
    FORCEINLINE void put_pixels(const Array2D<Color>& pixels);
    FORCEINLINE const Array2D<Color>& get_pixels() const { return pixels_; }
    void put_pixels(uint width, uint height, const List<Color>& pixels);

    Shared<Editor> edit();

    FORCEINLINE const ModuleAssetID& get_id() const { return id_; }

protected:
    void load() override;
    void unload() override;
    
private:
    void load_internal();
    void edit_count_increase();
    void edit_count_decrease();
    
    Array2D<Color> pixels_;
    uint gl_texture_binding_ = 0;
    uint64 handle_arb_ = 0;

    //static std::map<Texture*, uint> usage_counter_;
    uint edit_count_;
    bool delayed_activation_;

    Shared<Ogre::Texture> ogre_texture_;
    ModuleAssetID id_;
};

typedef Slot<Texture> TextureSlot;
