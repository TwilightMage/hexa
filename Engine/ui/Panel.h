#pragma once

#include "UIElement.h"
#include "Engine/Margins.h"
#include "Engine/Rect.h"

class Texture;
class Image;

class EXPORT Panel : public UIElement
{
public:
    Panel();
    Panel(const Shared<Texture>& texture, const Margins& texture_offsets = Margins());

    void use_texture(const Shared<Texture>& texture);
    const Rect& get_rect() const;
    void set_rect(const Rect& rect);
    void clear_rect();

protected:
    void on_construct() override;
    void on_size_changed() override;

private:
    void update_geometry() const;
    void update_texture() const;
    
    Margins margins_;
    Rect rect_;
    bool have_rect_;

    Shared<Image> lt_;
    Shared<Image> mt_;
    Shared<Image> rt_;
    Shared<Image> lm_;
    Shared<Image> mm_;
    Shared<Image> rm_;
    Shared<Image> lb_;
    Shared<Image> mb_;
    Shared<Image> rb_;

    Shared<Texture> texture_;
};
