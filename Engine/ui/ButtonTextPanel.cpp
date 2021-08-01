#include "ButtonTextPanel.h"

#include "Panel.h"
#include "TextBlock.h"
#include "Engine/Paths.h"
#include "Engine/Rect.h"
#include "Engine/SpriteFont.h"
#include "Engine/Texture.h"

ButtonTextPanel::ButtonTextPanel()
    : ButtonTextPanel("")
{
}

ButtonTextPanel::ButtonTextPanel(const String& text)
    : texture_(Texture::load_png(RESOURCES_TEXTURES_UI + "button_panel.png"))
    , text_(text)
{
}

void ButtonTextPanel::set_panel_texture(const Shared<Texture>& panel_texture)
{
    texture_ = panel_texture;
    if (is_started_construction())
    {
        panel_->use_texture(panel_texture);
    }
}

const String& ButtonTextPanel::get_text() const
{
    return text_;
}

void ButtonTextPanel::set_text(const String& text)
{
    if (text_ == text) return;
    
    text_ = text;

    if (is_started_construction())
    {
        text_block_->set_text(text);
        update_geometry();
    }
}

void ButtonTextPanel::on_construct()
{
    panel_ = MakeShared<Panel>(texture_, Margins(4, 4, 4, 4));
    panel_->set_mouse_detection(false);
    add_child(panel_);

    text_block_ = MakeShared<TextBlock>(text_);
    text_block_->set_font_size(12);
    panel_->add_child(text_block_);

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
    if (is_started_construction())
    {
        panel_->set_size(get_size());
        
        text_block_->set_position((get_size() - text_block_->get_size() - Vector2(0, text_block_->get_text_offset())) / 2.0f);
    }
}
