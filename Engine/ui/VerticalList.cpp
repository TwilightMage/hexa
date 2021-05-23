#include "VerticalList.h"

void VerticalList::on_parent_size_changed()
{
    for (auto& child : get_children())
    {
        child->set_size(Vector2(get_size().x, child->get_size().y));
    }
}

List<Vector2> VerticalList::arrange_positions(const List<Shared<UIElement>>& elements)
{
    List<Vector2> result(elements.length());

    Vector2 offset;
    for (uint i = 0; i < elements.length(); i++)
    {
        result[i] = offset;
        offset.y += elements[i]->get_size().y + get_spacing();
    }

    return result;
}
