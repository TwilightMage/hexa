#pragma once
#include "UIElement.h"
#include "Engine/BasicTypes.h"
#include "Engine/Delegate.h"

class EXPORT Button : public UIElement
{
public:
    Delegate<const Weak<Button>&> on_click;

protected:
    enum State
    {
        STATE_DEFAULT,
        STATE_PRESSED,
        STATE_HOVERED
    };

    void on_mouse_enter() override;
    void on_mouse_leave() override;
    void on_press() override;
    void on_release() override;
    
    void set_state(State state);
    State get_state() const;
    virtual void state_updated(State state);

private:
    State state_ = STATE_DEFAULT;
};
