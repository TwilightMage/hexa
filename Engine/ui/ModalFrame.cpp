#include "ModalFrame.h"

#include "Image.h"

ModalFrame::ModalFrame(const Shared<UIElement>& content)
    : content_(content)
{
}

Shared<ModalFrame> ModalFrame::show(const Shared<UIElement>& content, float z)
{
    Shared<ModalFrame> result = MakeShared<ModalFrame>(content);
    result->set_z(z);
    Game::add_ui(result);

    return result;
}

void ModalFrame::close()
{
    on_closed();
    remove_from_parent();
}

void ModalFrame::on_press(const Vector2& point)
{
    close();
}

void ModalFrame::on_size_changed()
{
    background_->set_size(get_size());
    
    if (content_)
    {
        content_->set_position((get_size() - content_->get_size()) / 2);
    }
}

void ModalFrame::on_parent_size_changed()
{
    if (const auto parent = get_parent())
    {
        set_size(parent->get_size());
    }
}

void ModalFrame::on_construct()
{
    background_ = MakeShared<Image>();
    background_->set_mouse_detection(false);
    background_->set_color(Color(0, 0, 0, 150));
    add_child(background_);

    on_size_changed();
}
