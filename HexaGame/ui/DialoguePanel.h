#pragma once

#include "Engine/ui/Panel.h"
#include "Engine/ui/TextBlock.h"
#include "Engine/ui/UIElement.h"

class DialoguePanel : public UIElement
{
public:

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
};
