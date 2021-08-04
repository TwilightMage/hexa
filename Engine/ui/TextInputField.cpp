#include "TextInputField.h"

#include "Image.h"
#include "Panel.h"
#include "TextBlock.h"
#include "Engine/Game.h"
#include "Engine/Margins.h"
#include "Engine/Paths.h"
#include "Engine/Regex.h"
#include "Engine/SpriteFont.h"
#include "Engine/Texture.h"

TextInputField::TextInputField()
    : TextInputField(20)
{
}

TextInputField::TextInputField(uint max_length)
    : texture_(Texture::load_png(RESOURCES_TEXTURES_UI + "input_panel.png"))
    , state_(STATE_UNFOCUSED)
    , max_length_(max_length)
{
}

void TextInputField::on_focus()
{
    state_ = STATE_FOCUSED;
    state_updated(state_);
    panel_->add_child(caret_);
}

void TextInputField::on_unfocus()
{
    state_ = STATE_UNFOCUSED;
    state_updated(state_);
    caret_->remove_from_parent();

    on_finish_edit(text_);
}

void TextInputField::key_down(int key)
{
    /*if (key == GLFW_KEY_BACKSPACE)
    {
        if (caret_position_ > 0)
        {
            text_ = text_.substring(0, caret_position_ - 1) + text_.substring(caret_position_);
            caret_position_--;
            update_caret_position();

            text_ui_->set_text(text_);
        }
    }
    else if (key == GLFW_KEY_DELETE)
    {
        if (caret_position_ < text_.length() && text_.length() > 0)
        {
            text_ = text_.substring(0, caret_position_) + text_.substring(caret_position_ + 1);

            text_ui_->set_text(text_);
        }
    }
    else if (key == GLFW_KEY_LEFT)
    {
        if (caret_position_ > 0)
        {
            caret_position_--;
            update_caret_position();
        }
    }
    else if (key == GLFW_KEY_RIGHT)
    {
        if (caret_position_ < text_.length())
        {
            caret_position_++;
            update_caret_position();
        }
    }
    else if (key == GLFW_KEY_UP)
    {
        if (caret_position_ > 0)
        {
            caret_position_ = 0;
            update_caret_position();
        }
    }
    else if (key == GLFW_KEY_DOWN)
    {
        if (caret_position_ < text_.length())
        {
            caret_position_ = text_.length();
            update_caret_position();
        }
    }
    else if (key == GLFW_KEY_ENTER)
    {
        Game::focus_ui(nullptr);
    }*/
}

void TextInputField::key_hold(int key)
{
    key_down(key);
}

void TextInputField::text_input(uint code)
{
    if (text_.length() < max_length_)
    {
        const char new_char = static_cast<char>(code);
        String new_text = text_.substring(0, caret_position_) + new_char + text_.substring(caret_position_);
        if (input_verifier.IsBound() ? input_verifier(new_text, new_char) : true)
        {
            text_ = new_text;

            text_ui_->set_text(text_);

            caret_position_++;
            update_caret_position();

            on_content_change(text_);
        }
    }
}

void TextInputField::set_text(const String& text)
{
    text_ = text;
    caret_position_ = text.length();

    if (is_started_construction())
    {
        text_ui_->set_text(text_);
        update_caret_position();
    }
}

bool TextInputField::IntegerInputVerifier(String& new_input, char new_char)
{
    return isdigit(new_char);
}

bool TextInputField::FloatInputVerifier(String& new_input, char new_char)
{
    if (isdigit(new_char))
    {
        return true;
    }
    if (new_char == '.')
    {
        return new_input.find(new_char).length() == 1;
    }

    return false;
}

void TextInputField::on_press(const Vector2& point)
{
    Game::get_instance()->focus_ui(cast<UIInputElement>(shared_from_this()));
}

void TextInputField::on_size_changed()
{
    if (is_started_construction())
    {
        panel_->set_size(get_size());
    }
}

void TextInputField::on_construct()
{
    panel_ = MakeShared<Panel>(texture_, Margins(4, 4, 4, 4));
    panel_->set_mouse_detection(false);
    add_child(panel_);

    text_ui_ = MakeShared<TextBlock>();
    text_ui_->set_position(Vector2(4, 2));
    panel_->add_child(text_ui_);

    caret_ = MakeShared<Image>(nullptr);
    caret_->set_size(Vector2(1, text_ui_->get_font_size() - 2));
    caret_->set_z(0.002f);
    caret_->set_color(Color(63, 63, 63));

    on_size_changed();
    state_updated(get_state());
    update_caret_position();
}

void TextInputField::state_updated(State state)
{
    panel_->set_rect(Rect(0, texture_->get_size().y / 2 * state_, texture_->get_size().x, texture_->get_size().y / 2));
}

void TextInputField::update_caret_position()
{
    caret_->set_position(Vector2(4 + text_ui_->get_font()->measure_string(text_.substring(0, caret_position_), text_ui_->get_font_size()) - 1, 3));
}
