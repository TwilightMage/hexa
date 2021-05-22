#pragma once

#include "UIElement.h"
#include "Engine/String.h"

class SpriteFont;

EXTERN class EXPORT TextBlock : public UIElement
{
public:
    TextBlock(const String& string);

    float get_font_size() const;
    void set_font_size(float font_size);
    const Shared<SpriteFont>& get_font() const;
    const String& get_text() const;
    void set_text(const String& text);
    bool get_wrap() const;
    void set_wrap(bool wrap);
    float get_text_offset() const;

protected:
    void on_construct() override;
    void on_size_changed() override;

private:
    void update_geometry();
    
    String text_;
    Shared<SpriteFont> font_;
    float font_size_;
    float font_scale_;
    bool wrap_;

    float text_offset_;
};
