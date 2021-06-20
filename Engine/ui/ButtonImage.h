#pragma once

#include "Button.h"
#include "UIElement.h"
#include "Engine/Delegate.h"

class Texture;
class Image;

class EXPORT ButtonImage : public Button
{
public:
    ButtonImage();
    ButtonImage(const Shared<Texture>& texture);

    void set_texture(const Shared<Texture>& texture);

    FORCEINLINE bool get_stretch_image() const { return stretch_image_; }
    void set_stretch_image(bool value);

protected:
    void on_construct() override;
    void on_size_changed() override;
    void state_updated(State state) override;
    
private:    
    Shared<Image> image_;
    Shared<Texture> texture_;
    bool stretch_image_ = false;
};
