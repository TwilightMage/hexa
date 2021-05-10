#pragma once

#include "UIElement.h"
#include "Engine/Delegate.h"

class Texture;
class Image;

EXTERN class EXPORT ButtonImage : public UIElement
{
public:
    ButtonImage(const Shared<Texture>& texture);
    Delegate<const Weak<UIElement>&> on_click;

protected:
    void on_construct() override;
    void on_mouse_enter() override;
    void on_mouse_leave() override;
    void on_press() override;
    void on_release() override;

private:
    void set_state(uint state);
    
    Shared<Image> image_;
    Shared<Texture> texture_;
    uint state_;
};
