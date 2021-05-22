#include "ButtonTextPanel.h"

#include "Panel.h"
#include "TextBlock.h"
#include "Engine/Rect.h"
#include "Engine/SpriteFont.h"
#include "Engine/Texture.h"

ButtonTextPanel::ButtonTextPanel()
    : texture_(Texture::load_png("resources/hexagame/textures/ui/button_panel.png"))
{
}

void ButtonTextPanel::set_text(const String& text)
{
    text_ = text;
    update_geometry();
}

void ButtonTextPanel::on_construct()
{
    panel_ = MakeShared<Panel>(texture_, Margins(8, 8, 8, 8));
    panel_->set_mouse_detection(false);
    add_child(panel_);

    text_block_ = MakeShared<TextBlock>(text_);
    text_block_->set_z(0.01f);
    add_child(text_block_);

    state_updated(get_state());
    update_geometry();
}

void ButtonTextPanel::on_size_changed()
{
    if (panel_)
    {
        update_geometry();
    }
}

void ButtonTextPanel::state_updated(State state)
{
    panel_->set_rect(Rect(0, texture_->get_height() / 3 * state, texture_->get_width(), texture_->get_height() / 3));
}

void ButtonTextPanel::update_geometry()
{
    if (panel_)
    {
        panel_->set_size(get_size());
    }

    if (text_block_)
    {
        text_block_->set_position((get_size() - text_block_->get_size() - Vector2(0, text_block_->get_text_offset())) / 2.0f);
    }
}
