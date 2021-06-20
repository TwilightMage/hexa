#include "ButtonImagePanel.h"

#include "Image.h"
#include "Panel.h"
#include "Engine/Margins.h"
#include "Engine/Paths.h"
#include "Engine/Texture.h"

ButtonImagePanel::ButtonImagePanel()
    : ButtonImagePanel(nullptr)
{
}

ButtonImagePanel::ButtonImagePanel(const Shared<Texture>& image_texture)
    : texture_(Texture::load_png(RESOURCES_ENGINE_TEXTURES_UI + "button_panel.png"))
    , image_texture_(image_texture)
{
}

void ButtonImagePanel::set_panel_texture(const Shared<Texture>& panel_texture)
{
    texture_ = panel_texture;
    if (is_started_construction())
    {
        panel_->use_texture(panel_texture);
    }
}

void ButtonImagePanel::set_image_texture(const Shared<Texture>& image_texture)
{
    image_texture_ = image_texture;
    if (is_started_construction())
    {
        image_->use_texture(image_texture);
    }
}


void ButtonImagePanel::on_construct()
{
    panel_ = MakeShared<Panel>(texture_, Margins(4, 4, 4, 4));
    panel_->set_mouse_detection(false);
    add_child(panel_);

    image_ = MakeShared<Image>();
    image_->set_mouse_detection(false);
    image_->use_texture(image_texture_);
    image_->set_z(0.001f);
    panel_->add_child(image_);

    state_updated(get_state());
    update_geometry();
}

void ButtonImagePanel::on_size_changed()
{
    if (panel_)
    {
        update_geometry();
    }
}

void ButtonImagePanel::state_updated(State state)
{
    panel_->set_rect(Rect(0, texture_->get_height() / 3 * state, texture_->get_width(), texture_->get_height() / 3));
}

void ButtonImagePanel::update_geometry()
{
    if (is_started_construction())
    {
        panel_->set_size(get_size());

        image_->set_size(image_texture_ ? image_texture_->get_size() : Vector2(10, 10));
        image_->set_position((get_size() - image_->get_size()) / 2.0f);
    }
}