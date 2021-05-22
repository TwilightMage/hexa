#pragma once

#include "Button.h"
#include "Engine/String.h"

class TextBlock;
class Panel;
class Texture;

EXTERN class EXPORT ButtonTextPanel : public Button
{
public:
    ButtonTextPanel();
    
    void set_text(const String& text);
    
protected:
    void on_construct() override;
    void on_size_changed() override;
    void state_updated(State state) override;
    
private:
    void update_geometry();
    
    Shared<Panel> panel_;
    Shared<Texture> texture_;
    Shared<TextBlock> text_block_;
    String text_;
};
