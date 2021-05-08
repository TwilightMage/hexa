#pragma once

#include "BasicTypes.h"
#include "Object.h"
#include "Path.h"
#include "TextureAtlas.h"

EXTERN class EXPORT SpriteFont : public Object
{
public:
    struct UILetter
    {
        Rect rect;
        const TextureAtlas::Entry* atlas_entry;
    };
    
    SpriteFont(const String& name);

    static Shared<SpriteFont> load_fnt(const Path& path);

    uint measure_string(const String& string) const;
    List<UILetter> arrange_string(const String& string) const;

    Shared<const TextureAtlas> get_atlas() const;
    uint get_line_height() const;
    uint get_base() const;

private:
    struct Letter
    {
        uint atlas_entry_id_;
        int offset_x;
        int offset_y;
        uint advance;
        uint width;
        uint height;
    };
    
    Shared<TextureAtlas> atlas_;
    uint line_height_;
    uint base_;
    std::map<char, Letter> entries_;
};
