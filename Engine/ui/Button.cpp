#include "Button.h"

void Button::on_mouse_enter()
{
    if (is_pressed())
    {
        set_state(STATE_PRESSED);
    }
    else
    {
        set_state(STATE_HOVERED);
    }
}

void Button::on_mouse_leave()
{
    set_state(STATE_DEFAULT);
}

void Button::on_press(const Vector2& point)
{
    set_state(STATE_PRESSED);
    on_click(cast<Button>(weak_from_this()));
}

void Button::on_release()
{
    if (is_mouse_over())
    {
        set_state(STATE_HOVERED);
    }
}

void Button::set_state(State state)
{
    state_ = state;
    state_updated(state_);
}

Button::State Button::get_state() const
{
    return state_;
}

void Button::state_updated(State state)
{
}
