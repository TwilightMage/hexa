#include "UIList.h"

float UIList::get_spacing() const
{
    return spacing_;
}

void UIList::set_spacing(float value)
{
    spacing_ = value;
    update_geometry();
}

void UIList::on_construct()
{
    update_geometry();
}

void UIList::on_child_added(const Shared<UIElement>& child)
{
    update_geometry();
}

void UIList::on_child_removed(const Shared<UIElement>& child)
{
    update_geometry();
}

void UIList::update_geometry()
{
    if (is_started_construction())
    {
        const auto positions = arrange_positions(get_children());
        for (uint i = 0; i < positions.length(); i++)
        {
            get_children()[i]->set_position(positions[i]);
        }
    }
}
