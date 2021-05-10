#pragma once

#include "Engine/ui/UIElement.h"

class TextBlock;
class Image;
class Panel;
class ButtonImage;

class DialoguePanel : public UIElement
{
protected:
    void on_size_changed() override;
    void on_parent_size_changed() override;

    void on_construct() override;

private:
    void update_geometry();
    
    Shared<Panel> panel_;
    Shared<Image> frame_;
    Shared<Image> avatar_;
    Shared<TextBlock> name_;
    Shared<TextBlock> message_;
    Shared<ButtonImage> close_button_;
};
