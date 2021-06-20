#include "ModalEnumFrame.h"

#include "ButtonTextPanel.h"
#include "Panel.h"
#include "TextBlock.h"
#include "Engine/Game.h"
#include "Engine/Math.h"

ModalEnumFrame::ModalEnumFrame(const List<String>& options)
    : ModalFrame(MakeShared<Panel>())
    , options_(options)
{
    panel_ = cast<Panel>(get_content());
}

Shared<ModalEnumFrame> ModalEnumFrame::show(const List<String>& options, float z)
{
    Shared<ModalEnumFrame> result = MakeShared<ModalEnumFrame>(options);
    result->set_z(z);
    Game::add_ui(result);

    return result;
}

void ModalEnumFrame::on_construct()
{
    ModalFrame::on_construct();
    
    panel_->set_size(Vector2(100, 200));
    add_child(panel_);

    for (uint i = 0; i < page_size; i++)
    {
        Shared<ButtonTextPanel> option_button = MakeShared<ButtonTextPanel>();
        option_button->set_position(Vector2(5, 20 * i + 5));
        option_button->set_size(Vector2(90, 15));
        option_button->on_click.bind(this, &ModalEnumFrame::option_selected);
        option_buttons_.Add(option_button);
        panel_->add_child(option_button);
    }

    prev_button_ = MakeShared<ButtonTextPanel>("<");
    prev_button_->set_size(Vector2(25, 15));
    prev_button_->set_position(Vector2(5, panel_->get_size().y - 20));
    prev_button_->on_click.bind(this, &ModalEnumFrame::prev_clicked);
    panel_->add_child(prev_button_);

    next_button_ = MakeShared<ButtonTextPanel>(">");
    next_button_->set_size(Vector2(25, 15));
    next_button_->set_position(Vector2(panel_->get_size().x - 30, panel_->get_size().y - 20));
    next_button_->on_click.bind(this, &ModalEnumFrame::next_clicked);
    panel_->add_child(next_button_);

    page_display_ = MakeShared<TextBlock>();
    panel_->add_child(page_display_);

    page_changed();
    on_size_changed();
}

void ModalEnumFrame::option_selected(const Weak<Button>& sender)
{
    if (const auto sender_lock = cast<ButtonTextPanel>(sender.lock()))
    {
        const uint id_on_page = option_buttons_.IndexOf(sender_lock);
        const uint selected_id = page_size * page + id_on_page;

        on_select(selected_id);
        on_select_name(options_[selected_id]);
        close();
    }
}

void ModalEnumFrame::prev_clicked(const Weak<Button>& sender)
{
    page--;
    page_changed();
}

void ModalEnumFrame::next_clicked(const Weak<Button>& sender)
{
    page++;
    page_changed();
}

void ModalEnumFrame::page_changed()
{
    const uint num_options = Math::min(page_size, options_.length() % page_size);
    const uint num_pages = Math::ceil(options_.length() / static_cast<float>(page_size));

    for (uint i = 0; i < num_options; i++)
    {
        option_buttons_[i]->set_text(options_[page * page_size + i]);
        panel_->add_child(option_buttons_[i]);
    }

    for (uint i = num_options; i < page_size; i++)
    {
        option_buttons_[i]->remove_from_parent();
    }

    if (page == 0)
    {
        prev_button_->remove_from_parent();
    }
    else
    {
        panel_->add_child(prev_button_);
    }

    if (page == num_pages - 1)
    {
        next_button_->remove_from_parent();
    }
    else
    {
        next_button_->add_child(prev_button_);
    }

    page_display_->set_text(String::format("%i/%i", page + 1, num_pages));
    page_display_->set_position(Vector2((panel_->get_size().x - page_display_->get_size().x) / 2, panel_->get_size().y - 20));
}
