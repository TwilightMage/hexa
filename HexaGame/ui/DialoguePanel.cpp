#include "DialoguePanel.h"


#include "Engine/Margins.h"
#include "Engine/Math.h"
#include "Engine/Texture.h"
#include "Engine/ui/ButtonImage.h"
#include "Engine/ui/ButtonTextPanel.h"
#include "Engine/ui/Image.h"
#include "Engine/ui/Panel.h"
#include "Engine/ui/TextBlock.h"
#include "HexaGame/Paths.h"

void DialoguePanel::on_size_changed()
{
    update_geometry();
}

void DialoguePanel::on_parent_size_changed()
{
    if (const auto parent = get_parent())
    {
        set_size(Vector2(Math::min(500.0f, parent->get_size().x - 20), 100));
        set_position(Vector2((parent->get_size().x - get_size().x) / 2, parent->get_size().y - 110));
    }
}

void DialoguePanel::on_construct()
{    
    panel_ = MakeShared<Panel>();
    add_child(panel_);
    
    frame_ = MakeShared<Image>(Texture::load_png(RESOURCES_TEXTURES_UI +  "frame.png"));
    frame_->set_position(Vector2(10.0f, 10.0f));
    frame_->set_size(Vector2(38.0f, 38.0f) * 2.0f);
    frame_->set_z(0.1f);
    panel_->add_child(frame_);
    
    avatar_ = MakeShared<Image>(Texture::load_png(RESOURCES_TEXTURES_UI + "dragon.png"));
    avatar_->set_position(Vector2(3.0f, 3.0f) * 2.0f);
    avatar_->set_size(Vector2(32.0f, 32.0f) * 2.0f);
    frame_->add_child(avatar_);
    
    name_ = MakeShared<TextBlock>("Orachir, the Dragon");
    name_->set_position(Vector2(95.0f, 5.0f));
    name_->set_font_size(24);
    panel_->add_child(name_);
    
    message_ = MakeShared<TextBlock>("Well, well, well... Look who came. What do you want to ask me this time?");
    message_->set_wrap(true);
    message_->set_font_size(12);
    message_->set_position(Vector2(95.0f, 30.0f));
    message_->set_z(0.1f);
    panel_->add_child(message_);

    button_ = MakeShared<ButtonTextPanel>();
    button_->set_text("Push me!");
    button_->set_size(Vector2(75.0f, 20.0f));
    button_->on_click.bind(this, &DialoguePanel::push_button);
    panel_->add_child(button_);
}

void DialoguePanel::update_geometry()
{
    if (is_started_construction())
    {
        panel_->set_size(get_size());

        name_->set_size(Vector2(panel_->get_size().x - 95 - 10, 85.0f));
	
        message_->set_size(Vector2(panel_->get_size().x - 95 - 10, 85.0f));

        button_->set_position(Vector2(get_size().x - 85, get_size().y - 30));
    }
}

void DialoguePanel::push_button(const Weak<Button>& btn)
{
    message_->set_text(String::format("Well, well, well... Someone pushed the button %i times?", ++push_counter_));
}
