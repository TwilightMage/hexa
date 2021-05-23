#pragma once
#include "UIList.h"

EXTERN class EXPORT VerticalList : public UIList
{
protected:
    void on_parent_size_changed() override;
    List<Vector2> arrange_positions(const List<Shared<UIElement>>& elements) override;
};
