#pragma once

#include "UIElement.h"
#include "Engine/Margins.h"

class Texture;
class Image;

class Panel : public UIElement
{
public:
    Panel(const Margins& texture_offsets = Margins());
    
protected:
    void on_construct() override;
    void on_size_changed() override;

private:
    void update_local_geometry() const;
    void update_texture(const Shared<Texture>& texture);
    
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
