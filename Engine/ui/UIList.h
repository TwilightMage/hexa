#pragma once

#include "UIElement.h"

EXTERN class EXPORT UIList : public UIElement
{
public:
    float get_spacing() const;
    void set_spacing(float value);
    
protected:
    void on_construct() override;
    void on_child_added(const Shared<UIElement>& child) override;
    void on_child_removed(const Shared<UIElement>& child) override;
    
    virtual List<Vector2> arrange_positions(const List<Shared<UIElement>>& elements) = 0;

private:
    void update_geometry();

    float spacing_ = 0;
};
