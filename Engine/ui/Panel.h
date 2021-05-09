#pragma once

#include "UIElement.h"
#include "Engine/Margins.h"

class Texture;
class Image;

EXTERN class EXPORT Panel : public UIElement
{
public:
    Panel(const Shared<Texture>& texture, const Margins& texture_offsets = Margins());

    void use_texture(const Shared<Texture>& texture);

protected:
    void on_construct() override;
    void on_size_changed() override;

private:
    void update_geometry() const;
    void update_texture() const;
    
    Margins margins_;

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
