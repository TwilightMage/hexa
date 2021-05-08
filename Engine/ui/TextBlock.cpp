#include "TextBlock.h"


#include "Image.h"
#include "Engine/Game.h"
#include "Engine/SpriteFont.h"

TextBlock::TextBlock(const String& string)
    : string_(string)
{
    font_ = Game::get_font_harrington();
    scale_ = 32.0f / font_->get_line_height();
}

void TextBlock::on_construct()
{
    auto tex = font_->get_atlas()->to_texture();
    if (const auto editor = tex->edit())
    {
        editor->process_pixels([](uint x, uint y, const Color& color) -> Color
        {
            return Color(255, 255, 255, color.r);
        });
    }
    
    auto letters = font_->arrange_string(string_);

    for (const auto& let : letters)
    {
        auto ui_let = MakeShared<Image>();
        ui_let->set_position(Vector2(static_cast<float>(let.rect.x) * scale_, static_cast<float>(let.rect.y) * scale_));
        ui_let->set_size(Vector2(static_cast<float>(let.rect.w) * scale_, static_cast<float>(let.rect.h) * scale_));
        ui_let->use_texture(tex);
        ui_let->set_rect(let.atlas_entry->rect);
        add_child(ui_let);
    }
}
