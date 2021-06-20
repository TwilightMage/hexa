#pragma once

#include "Engine/Curve.h"
#include "Engine/Delegate.h"
#include "Engine/ui/ButtonImage.h"

class Image;
class Texture;

class CurveSegmentWidget : public ButtonImage
{
public:
    void on_construct() override;

    void on_size_changed() override;

    void update_texture();
    
    FORCEINLINE const Shared<Texture>& get_texture() const { return texture_; }
    FORCEINLINE const Shared<Texture>& get_texture_active() const { return texture_active_; }

    const Curve<float>::Point* p1;
    const Curve<float>::Point* p2;
    float offset;
    float scale;
    Shared<Curve<float>> curve;
    String curve_name;
    uint segment_id;

    Delegate<const Weak<CurveSegmentWidget>&> on_click;

protected:
    void on_press() override;

private:
    void update_geometry();
    
    Shared<Texture> texture_;
    Shared<Texture> texture_active_;
};
