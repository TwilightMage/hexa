#pragma once

#include "UIElement.h"
#include "Engine/Margins.h"

class Texture;
class Image;

class Panel : public UIElement
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

    Image* lt_;
    Image* mt_;
    Image* rt_;
    Image* lm_;
    Image* mm_;
    Image* rm_;
    Image* lb_;
    Image* mb_;
    Image* rb_;

    Shared<Texture> texture_;
};
