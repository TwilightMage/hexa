#include "SpriteFont.h"

#include "File.h"
#include "Game.h"

std::pmr::map<String, String> parse_params(const String& line)
{
    std::pmr::map<String, String> result;

    auto state = false;
    auto quoted = false;
    String key, val;
    for (auto& ch : line)
    {
        if (!quoted)
        {
            if (ch == ' ')
            {
                if (state)
                {
                    result[key] = val;
                    state = false;
                }
                key = val = "";
            }
            else if (ch == '"')
            {
                quoted = true;
            }
            else if (ch == '=')
            {
                state = !state;
            }
            else if (state)
            {
                val += ch;
            }
            else
            {
                key += ch;
            }
        }
        else
        {
            if (ch == '"')
            {
                quoted = false;
            }
            else if (state)
            {
                val += ch;
            }
            else
            {
                key += ch;
            }
        }
    }

    result[key] = val;

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

                    assert_error(!info_params.contains("charset") || info_params["charset"] == "ANSI", nullptr, "Unsupported charset %s in font %s", info_params["charset"].c(), path.get_absolute_string().c());

                    assert_error(!info_params.contains("unicode") || info_params["unicode"] == "0", nullptr, "Unicode is not supported in font %s", path.get_absolute_string().c());
                }
                else if (line.starts_with("common "))
                {
                    auto info_params = parse_params(line);

                    assert_error(!info_params.contains("pages") || info_params["pages"] == "1", nullptr, "Unsupported number of pages in font %s", path.get_absolute_string().c());

                    assert_error(info_params.contains("lineHeight"), nullptr, "Missing parameter lineHeight in font %s", path.get_absolute_string().c());
                    result->line_height_ = StringParse<uint>(info_params["lineHeight"]);

                    assert_error(info_params.contains("base"), nullptr, "Missing parameter base in font %s", path.get_absolute_string().c());
                    result->base_ = StringParse<uint>(info_params["base"]);
                }
                else if (line.starts_with("page "))
                {
                    auto info_params = parse_params(line);

                    assert_error(info_params.contains("file"), nullptr, "Missing parameter file in font %s", path.get_absolute_string().c());
                    
                    result->atlas_ = TextureAtlas::load_png(path.up().get_child(info_params["file"]));
                }
                else if (line.starts_with("char "))
                {
                    auto info_params = parse_params(line);

                    assert_error(info_params.contains("id"), nullptr, "Missing parameter id in font %s", path.get_absolute_string().c());
                    assert_error(info_params.contains("x"), nullptr, "Missing parameter x in font %s", path.get_absolute_string().c());
                    assert_error(info_params.contains("y"), nullptr, "Missing parameter y in font %s", path.get_absolute_string().c());
                    assert_error(info_params.contains("width"), nullptr, "Missing parameter width in font %s", path.get_absolute_string().c());
                    assert_error(info_params.contains("height"), nullptr, "Missing parameter height in font %s", path.get_absolute_string().c());
                    assert_error(info_params.contains("xadvance"), nullptr, "Missing parameter xadvance in font %s", path.get_absolute_string().c());

                    Letter letter;
                    const auto ch = static_cast<char>(StringParse<int>(info_params["id"]));
                    letter.atlas_entry_id_ = result->atlas_->claim_rect({StringParse<int>(info_params["x"]), StringParse<int>(info_params["y"]), StringParse<int>(info_params["width"]), StringParse<int>(info_params["height"])});
                    assert_error(letter.atlas_entry_id_ != -1, nullptr, "Unable to claim rect on atlas for letter %c in font %s", ch, path.get_absolute_string().c());
                    letter.advance = StringParse<int>(info_params["xadvance"]);
                    letter.offset_x = StringParse<int>(info_params["xoffset"]);
                    letter.offset_y = StringParse<int>(info_params["yoffset"]);
                    letter.width = StringParse<int>(info_params["width"]);
                    letter.height = StringParse<int>(info_params["height"]);

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

uint SpriteFont::measure_string(const String& string) const
{
    uint result = 0;
    for (auto ch : string)
    {
        const auto letter = entries_.contains(ch) ? entries_.at(ch) : entries_.at('?');
        result += letter.advance + 1;
    }

    return result;
}

uint SpriteFont::measure_char(char ch) const
{
    return (entries_.contains(ch) ? entries_.at(ch) : entries_.at('?')).advance + 1;
}

List<SpriteFont::UILetter> SpriteFont::arrange_string(const String& string, uint& out_length) const
{
    out_length = 0;
    List<UILetter> result;
    uint ptr = 0;
    for (auto ch : string)
    {
        const auto letter = entries_.contains(ch) ? entries_.at(ch) : entries_.at('?');
        result.Add({{static_cast<int>(ptr + letter.offset_x), static_cast<int>(letter.offset_y), static_cast<int>(letter.width), static_cast<int>(letter.height)}, atlas_->get_entry(letter.atlas_entry_id_), ch});
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
                result.Add({{static_cast<int>(current_length + space_letter.offset_x), static_cast<int>(space_letter.offset_y), static_cast<int>(space_letter.width), static_cast<int>(space_letter.height)}, atlas_->get_entry(space_letter.atlas_entry_id_), ' '});
                for (auto& letter : word) letter.rect.x += current_length + space_letter.advance + 1;
                result.AddMany(word);
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
