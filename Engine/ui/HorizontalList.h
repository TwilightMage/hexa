﻿#pragma once
#include "UIList.h"

EXTERN class EXPORT HorizontalList : public UIList
{
protected:
    void on_parent_size_changed() override;
    List<Vector2> arrange_positions(const List<Shared<UIElement>>& elements) override;
};