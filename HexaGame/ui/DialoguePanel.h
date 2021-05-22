#pragma once

#include "Engine/ui/UIElement.h"

class Button;
class TextBlock;
class Image;
class Panel;
class ButtonTextPanel;

class DialoguePanel : public UIElement
{
protected:
    void on_size_changed() override;
    void on_parent_size_changed() override;

    void on_construct() override;

private:
    void update_geometry();
    void push_button(const Weak<Button>& btn);
    uint push_counter_ = 0;
    
    Shared<Panel> panel_;
    Shared<Image> frame_;
    Shared<Image> avatar_;
    Shared<TextBlock> name_;
    Shared<TextBlock> message_;
    Shared<ButtonTextPanel> button_;
};
