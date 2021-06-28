#pragma once

#include "Inventory.h"

class CharacterInventory : public Inventory
{
public:
    explicit CharacterInventory(uint size);
    
    FORCEINLINE uint get_selected_hotbar() const { return selected_hotbar_; }
    void set_selected_hotbar(uint selection);

    const ItemContainer& get_selected_item() const;
    
    Delegate<uint> on_hotbar_select;

private:
    uint selected_hotbar_ = 0;

    List<ItemContainer> equipment;
};
