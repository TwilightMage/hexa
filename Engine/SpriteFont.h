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
        char character;
    };
    
    SpriteFont(const String& name);

    static Shared<SpriteFont> load_fnt(const Path& path);

    uint measure_string(const String& string) const;
    uint measure_char(char ch) const;
    List<UILetter> arrange_string(const String& string, uint& out_length) const;
    List<UILetter> arrange_line(const String& whole_text, uint line_width) const;

    Shared<Texture> get_atlas_texture() const;
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
    Shared<Texture> atlas_texture_;
    uint line_height_;
    uint base_;
    std::map<char, Letter> entries_;
};
