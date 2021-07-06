#include "SpriteFont.h"

#include "Assert.h"
#include "File.h"
#include "Map.h"

Map<String, String> parse_params(const String& line)
{
    if (line.length() == 0) return Map<String, String>();
    
    Map<String, String> result;

    auto parts = line.split(' ', true);
    
    for (uint i = 1; i < parts.length(); i++)
    {
        auto fragments = parts[i].split('=');
        result[fragments[0]] = fragments[1].trim("\"");
    }

    return result;
}

SpriteFont::SpriteFont(const String& name)
    : Object(name)
{
}

Shared<SpriteFont> SpriteFont::load_fnt(const Path& path)
{
    if (path.exists())
    {
        if (auto reader = File::Reader::open(path))
        {
            auto result = MakeShared<SpriteFont>(path.get_absolute_string());
            
            while (!reader->is_end_of_file())
            {
                auto line = reader->read_line();
                if (line.starts_with("info "))
                {
                    auto info_params = parse_params(line);
                    
                    /*if (info_params.contains("face"))
                    {
                        result->name = info_params["face"] + " Font";
                    }*/

                    if (!Check(info_params.contains("charset") && info_params["charset"] == "ANSI", "Font Loader", "Unsupported charset %s in font %s", info_params["charset"].c(), path.get_absolute_string().c())) return nullptr;

                    if (!Check(!info_params.contains("unicode") || info_params["unicode"] == "0", "Font Loader", "Unicode is not supported in font %s", path.get_absolute_string().c())) return nullptr;
                }
                else if (line.starts_with("common "))
                {
                    auto info_params = parse_params(line);

                    if (!Check(!info_params.contains("pages") || info_params["pages"] == "1", "Font Loader", "Unsupported number of pages in font %s", path.get_absolute_string().c())) return nullptr;

                    if (!Check(info_params.contains("lineHeight"), "Font Loader", "Missing parameter lineHeight in font %s", path.get_absolute_string().c())) return nullptr;
                    result->line_height_ = String::parse<uint>(info_params["lineHeight"]);

                    if (!Check(info_params.contains("base"), "Font Loader", "Missing parameter base in font %s", path.get_absolute_string().c())) return nullptr;
                    result->base_ = String::parse<uint>(info_params["base"]);
                }
                else if (line.starts_with("page "))
                {
                    auto info_params = parse_params(line);

                    if (!Check(info_params.contains("file"), "Font Loader", "Missing parameter file in font %s", path.get_absolute_string().c())) return nullptr;
                    
                    result->atlas_ = TextureAtlas::load_png(path.up().get_child(info_params["file"]));
                }
                else if (line.starts_with("char "))
                {
                    auto info_params = parse_params(line);

                    if (!Check(info_params.contains("id"), "Font Loader", "Missing parameter id in font %s", path.get_absolute_string().c())) return nullptr;
                    if (!Check(info_params.contains("x"), "Font Loader", "Missing parameter x in font %s", path.get_absolute_string().c())) return nullptr;
                    if (!Check(info_params.contains("y"), "Font Loader", "Missing parameter y in font %s", path.get_absolute_string().c())) return nullptr;
                    if (!Check(info_params.contains("width"), "Font Loader", "Missing parameter width in font %s", path.get_absolute_string().c())) return nullptr;
                    if (!Check(info_params.contains("height"), "Font Loader", "Missing parameter height in font %s", path.get_absolute_string().c())) return nullptr;
                    if (!Check(info_params.contains("xadvance"), "Font Loader", "Missing parameter xadvance in font %s", path.get_absolute_string().c())) return nullptr;

                    Letter letter;
                    const auto ch = static_cast<char>(String::parse<int>(info_params["id"]));
                    letter.atlas_entry_id_ = result->atlas_->claim_rect({String::parse<int>(info_params["x"]), String::parse<int>(info_params["y"]), String::parse<int>(info_params["width"]), String::parse<int>(info_params["height"])});
                    if (!Check(letter.atlas_entry_id_ != -1, "Font Loader", "Unable to claim rect on atlas for letter %c in font %s", ch, path.get_absolute_string().c())) return nullptr;
                    letter.advance = String::parse<int>(info_params["xadvance"]);
                    letter.offset_x = String::parse<int>(info_params["xoffset"]);
                    letter.offset_y = String::parse<int>(info_params["yoffset"]);
                    letter.width = String::parse<int>(info_params["width"]);
                    letter.height = String::parse<int>(info_params["height"]);

                    result->entries_[ch] = letter;
                }
            }

            result->atlas_texture_ = result->atlas_->to_texture();
            if (const auto editor = result->atlas_texture_->edit())
            {
                editor->process_pixels([](uint x, uint y, const Color& color) -> Color
                {
                    return Color(255, 255, 255, color.r);
                });
            }
            return result;
        }
    }

    return nullptr;
}

uint SpriteFont::measure_string(const String& string, float font_size) const
{
    float result = 0;
    for (auto ch : string)
    {
        const auto letter = entries_.contains(ch) ? entries_.at(ch) : entries_.at('?');
        result += (letter.advance + 1) * font_size / line_height_;
    }

    return static_cast<uint>(result);
}

uint SpriteFont::measure_char(char ch, float font_size) const
{
    return static_cast<uint>(((entries_.contains(ch) ? entries_.at(ch) : entries_.at('?')).advance + 1) * font_size / line_height_);
}

List<SpriteFont::UILetter> SpriteFont::arrange_string(const String& string, uint& out_length) const
{
    out_length = 0;
    List<UILetter> result;
    uint ptr = 0;
    for (auto ch : string)
    {
        const auto letter = entries_.contains(ch) ? entries_.at(ch) : entries_.at('?');
        result.add({{static_cast<int>(ptr + letter.offset_x), static_cast<int>(letter.offset_y), static_cast<int>(letter.width), static_cast<int>(letter.height)}, atlas_->get_entry(letter.atlas_entry_id_), ch});
        ptr += letter.advance + 1;
    }
    out_length = ptr;
    
    return result;
}

List<SpriteFont::UILetter> SpriteFont::arrange_line(const String& whole_text, uint line_width) const
{
    uint current_length = 0;
    const auto space_letter = entries_.contains(' ') ? entries_.at(' ') : entries_.at('?');
    auto words = whole_text.split(' ', true);
    if (words.length() > 0)
    {
        uint i = 0;
        uint word_length;
        List<UILetter> result = arrange_string(words[i++], word_length);
        current_length = word_length;
        while (i < words.length())
        {
            auto word = arrange_string(words[i++], word_length);
            if (current_length + space_letter.advance + 1 + word_length < line_width)
            {
                result.add({{static_cast<int>(current_length + space_letter.offset_x), static_cast<int>(space_letter.offset_y), static_cast<int>(space_letter.width), static_cast<int>(space_letter.height)}, atlas_->get_entry(space_letter.atlas_entry_id_), ' '});
                for (auto& letter : word) letter.rect.x += current_length + space_letter.advance + 1;
                result.add_many(word);
                current_length += space_letter.advance + 1 + word_length;
            }
            else
            {
                break;
            }
        }

        return result;
    }

    return List<UILetter>();
}

Shared<Texture> SpriteFont::get_atlas_texture() const
{
    return atlas_texture_;
}

uint SpriteFont::get_line_height() const
{
    return line_height_;
}

uint SpriteFont::get_base() const
{
    return base_;
}
