#pragma once

#include "Button.h"
#include "UIElement.h"
#include "Engine/Delegate.h"

class Texture;
class Image;

EXTERN class EXPORT ButtonImage : public Button
{
public:
    ButtonImage(const Shared<Texture>& texture);

protected:
    void on_construct() override;
    void on_size_changed() override;
    void state_updated(State state) override;
    
private:    
    Shared<Image> image_;
    Shared<Texture> texture_;
};
