#pragma once

#include "Button.h"

class Image;
class Texture;
class Panel;

class ButtonImagePanel : public Button
{
public:
    ButtonImagePanel();
    explicit ButtonImagePanel(const Shared<Texture>& image_texture);

    void set_panel_texture(const Shared<Texture>& panel_texture);
    void set_image_texture(const Shared<Texture>& image_texture);

protected:
    void on_construct() override;
    void on_size_changed() override;
    void state_updated(State state) override;
    
private:
    void update_geometry();
    
    Shared<Panel> panel_;
    Shared<Texture> texture_;
    Shared<Image> image_;
    Shared<Texture> image_texture_;
};
