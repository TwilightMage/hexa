#include "TextBlock.h"

#include "Image.h"
#include "Engine/Game.h"
#include "Engine/Math.h"
#include "Engine/SpriteFont.h"

TextBlock::TextBlock()
    : TextBlock("")
{
}

TextBlock::TextBlock(const String& string)
    : text_(string)
    , font_size_(12)
    , wrap_(false)
{
    font_ = Game::get_default_font();
    font_scale_ = font_size_ / font_->get_line_height();
    set_mouse_detection(false);
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

const Shared<SpriteFont>& TextBlock::get_font() const
{
    return font_;
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

float TextBlock::get_text_offset() const
{
    return text_offset_;
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
        text_offset_ = std::numeric_limits<float>::max();
        
        if (wrap_)
        {
            float text_height = 0;
            float line_y = 0;
            String text_to_arrange = text_;
            while (text_to_arrange.length() > 0)
            {
                auto letters = font_->arrange_line(text_to_arrange, static_cast<uint>(get_size().x / font_scale_));
                float z = get_position().z;

                bool ping_pong = true;
                for (const auto& let : letters)
                {
                    auto ui_let = MakeShared<Image>();
                    ui_let->set_position(Vector3(static_cast<float>(let.rect.x) * font_scale_, static_cast<float>(let.rect.y) * font_scale_ + line_y, z));
                    ui_let->set_size(Vector2(static_cast<float>(let.rect.w) * font_scale_, static_cast<float>(let.rect.h) * font_scale_));
                    ui_let->use_texture(font_->get_atlas_texture());
                    ui_let->set_rect(let.atlas_entry->rect);
                    ui_let->set_mouse_detection(false);
                    add_child(ui_let);

                    z += ping_pong ? 0.0001f : -0.0001f;
                    ping_pong = !ping_pong;

                    text_offset_ = Math::min(text_offset_, ui_let->get_position().y);
                    text_height = Math::max(text_height, ui_let->get_position().y + ui_let->get_size().y);
                }

                line_y += static_cast<float>(font_->get_line_height()) * font_scale_;
                text_to_arrange = text_to_arrange.substring(letters.length()).trim_start();

                set_size_internal(Vector2(get_size().x, text_height));
            }
        }
        else
        {
            Vector2 text_size;
            uint len;
            auto letters = font_->arrange_string(text_, len);
            float z = get_position().z;

            bool ping_pong = true;
            for (const auto& let : letters)
            {
                auto ui_let = MakeShared<Image>();
                ui_let->set_position(Vector3(static_cast<float>(let.rect.x) * font_scale_, static_cast<float>(let.rect.y) * font_scale_, z));
                ui_let->set_size(Vector2(static_cast<float>(let.rect.w) * font_scale_, static_cast<float>(let.rect.h) * font_scale_));
                ui_let->use_texture(font_->get_atlas_texture());
                ui_let->set_rect(let.atlas_entry->rect);
                ui_let->set_mouse_detection(false);
                add_child(ui_let);
                
                z += ping_pong ? 0.0001f : -0.0001f;
                ping_pong = !ping_pong;

                text_offset_ = Math::min(text_offset_, ui_let->get_position().y);
                text_size.x = Math::max(text_size.x, ui_let->get_position().x + ui_let->get_size().x);
                text_size.y = Math::max(text_size.y, ui_let->get_position().y + ui_let->get_size().y);
            }

            set_size_internal(text_size);
        }

        if (text_offset_ == std::numeric_limits<float>::max())
        {
            text_offset_ = 0;
        }
    }
}
