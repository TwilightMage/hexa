#include "DialoguePanel.h"


#include "Engine/Margins.h"
#include "Engine/Math.h"
#include "Engine/Texture.h"
#include "Engine/ui/ButtonImage.h"
#include "Engine/ui/ButtonTextPanel.h"
#include "Engine/ui/Image.h"
#include "Engine/ui/Panel.h"
#include "Engine/ui/TextBlock.h"

void DialoguePanel::on_size_changed()
{
    update_geometry();
}

void DialoguePanel::on_parent_size_changed()
{
    if (auto parent = get_parent().lock())
    {
        set_size(Vector2(Math::min(1080.0f, parent->get_size().x - 20), 200));
        set_position(Vector2((parent->get_size().x - get_size().x) / 2, parent->get_size().y - 210));
    }
}

void DialoguePanel::on_construct()
{    
    panel_ = MakeShared<Panel>(Texture::load_png("resources/hexagame/textures/ui/panel.png"), Margins(8, 8, 8, 8));
    add_child(panel_);
    
    frame_ = MakeShared<Image>(Texture::load_png("resources/hexagame/textures/ui/frame.png"));
    frame_->set_z(0.2f);
    panel_->add_child(frame_);
    
    avatar_ = MakeShared<Image>(Texture::load_png("resources/hexagame/textures/ui/dragon.png"));
    avatar_->set_z(0.1f);
    frame_->add_child(avatar_);
    
    name_ = MakeShared<TextBlock>("Orachir, the Dragon");
    name_->set_z(0.1f);
    name_->set_font_size(48);
    panel_->add_child(name_);
    
    message_ = MakeShared<TextBlock>("Well, well, well... Look who came. What do you want to ask me this time?");
    message_->set_wrap(true);
    message_->set_font_size(24);
    message_->set_z(0.1f);
    panel_->add_child(message_);

    button_ = MakeShared<ButtonTextPanel>();
    button_->set_text("Push me!");
    button_->set_size(Vector2(150.0f, 40.0f));
    button_->on_click.bind(this, &DialoguePanel::push_button);
    panel_->add_child(button_);
    
    on_parent_size_changed();
}

void DialoguePanel::update_geometry()
{
    if (is_started_construction())
    {
        panel_->set_size(get_size());

        frame_->set_position(Vector2(20.0f, 20.0f));
        frame_->set_size(Vector2(76.0f, 76.0f) * 2.0f);

        avatar_->set_position(Vector2(6.0f, 6.0f) * 2.0f);
        avatar_->set_size(Vector2(64.0f, 64.0f) * 2.0f);

        name_->set_size(Vector2(panel_->get_size().x - 190 - 20, 170.0f));
        name_->set_position(Vector2(190.0f, 10.0f));
	
        message_->set_size(Vector2(panel_->get_size().x - 190 - 20, 170.0f));
        message_->set_position(Vector2(190.0f, 60.0f));

        button_->set_position(Vector2(get_size().x - 170, get_size().y - 60));
    }
}

void DialoguePanel::push_button(const Weak<Button>& btn)
{
    message_->set_text(StringFormat("Well, well, well... Someone pushed the button %i times?", ++push_counter_));
}
