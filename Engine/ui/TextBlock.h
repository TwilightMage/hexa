#pragma once

#include "UIElement.h"
#include "Engine/String.h"

class SpriteFont;

class TextBlock : public UIElement
{
public:
    TextBlock(const String& string);

protected:
    void on_construct() override;

private:
    String string_;
    Shared<SpriteFont> font_;
    float scale_;
};
