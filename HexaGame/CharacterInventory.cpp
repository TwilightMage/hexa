#include "CharacterInventory.h"

CharacterInventory::CharacterInventory(uint size)
    : Inventory(size)
{
}

void CharacterInventory::set_selected_hotbar(uint selection)
{
    if (selected_hotbar_ == selection) return;

    selected_hotbar_ = selection;
    on_hotbar_select(selection);
}

const ItemContainer& CharacterInventory::get_selected_item() const
{
    return get_item(selected_hotbar_);
}
