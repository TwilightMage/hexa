#include "HorizontalList.h"

void HorizontalList::on_parent_size_changed()
{
    for (auto& child : get_children())
    {
        child->set_size(Vector2(child->get_size().x, get_size().y));
    }
}

List<Vector2> HorizontalList::arrange_positions(const List<Shared<UIElement>>& elements)
{
    List<Vector2> result(elements.length());

    Vector2 offset;
    for (uint i = 0; i < elements.length(); i++)
    {
        result.add(offset);
        offset.x += elements[i]->get_size().x + get_spacing();
    }

    return result;
}
