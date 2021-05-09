#include "TextBlock.h"


#include "Image.h"
#include "Engine/Game.h"
#include "Engine/SpriteFont.h"

TextBlock::TextBlock(const String& string)
    : text_(string)
    , font_size_(24)
{
    font_ = Game::get_default_font();
    font_scale_ = font_size_ / font_->get_line_height();
}

float TextBlock::get_font_size() const
{
    return font_size_;
}

void TextBlock::set_font_size(float font_size)
{
    if (font_size_ != font_size)
    {
        font_size_ = font_size;
        font_scale_ = font_size_ / font_->get_line_height();
        update_geometry();
    }
}

const String& TextBlock::get_text() const
{
    return text_;
}

void TextBlock::set_text(const String& text)
{
    if (text_ != text)
    {
        text_ = text;
        update_geometry();
    }
}

bool TextBlock::get_wrap() const
{
    return wrap_;
}

void TextBlock::set_wrap(bool wrap)
{
    if (wrap_ != wrap)
    {
        wrap_ = wrap;
        update_geometry();
    }
}

void TextBlock::on_construct()
{
    update_geometry();
}

void TextBlock::on_size_changed()
{
    if (wrap_)
    {
        update_geometry();
    }
}

void TextBlock::update_geometry()
{
    if (is_started_construction())
    {
        remove_all_children();

        if (wrap_)
        {
            float line_y = 0;
            String text_to_arrange = text_;
            while (text_to_arrange.length() > 0)
            {
                auto letters = font_->arrange_line(text_to_arrange, static_cast<uint>(get_size().x / font_scale_));
                float z = get_position().z;
                
                for (const auto& let : letters)
                {
                    auto ui_let = MakeShared<Image>();
                    ui_let->set_position(Vector3(static_cast<float>(let.rect.x) * font_scale_, static_cast<float>(let.rect.y) + line_y * font_scale_, z));
                    ui_let->set_size(Vector2(static_cast<float>(let.rect.w) * font_scale_, static_cast<float>(let.rect.h) * font_scale_));
                    ui_let->use_texture(font_->get_atlas_texture());
                    ui_let->set_rect(let.atlas_entry->rect);
                    add_child(ui_let);
                    z += 0.0001f;
                }

                line_y += static_cast<float>(font_->get_line_height()) * font_scale_;
                text_to_arrange = text_to_arrange.substring(letters.length()).trim_start();
            }
        }
        else
        {
            uint len;
            auto letters = font_->arrange_string(text_, len);
            float z = get_position().z;
            
            for (const auto& let : letters)
            {
                auto ui_let = MakeShared<Image>();
                ui_let->set_position(Vector3(static_cast<float>(let.rect.x) * font_scale_, static_cast<float>(let.rect.y) * font_scale_, z));
                ui_let->set_size(Vector2(static_cast<float>(let.rect.w) * font_scale_, static_cast<float>(let.rect.h) * font_scale_));
                ui_let->use_texture(font_->get_atlas_texture());
                ui_let->set_rect(let.atlas_entry->rect);
                add_child(ui_let);
                z += 0.0001f;
            }
        }
    }
}
