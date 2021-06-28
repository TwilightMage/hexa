#pragma once

#include "ItemContainer.h"
#include "Engine/BasicTypes.h"
#include "Engine/Delegate.h"
#include "Engine/List.h"
#include "Engine/Map.h"

class Inventory
{
public:
    explicit Inventory(uint size);
    
    FORCEINLINE uint get_size() const { return items_.length(); }

    const ItemContainer& get_item(uint index) const;
    void set_item(uint index, const ItemContainer& item);
    bool add_item(const ItemContainer& item);        // try to add item, otherwise do nothing
    uint add_item_force(const ItemContainer& item);  // fill inventory with item, returns rest count
    bool take_item(const ItemContainer& item);       // try to take item, otherwise do nothing
    uint take_item_force(const ItemContainer& item); // take as much as possible, returns count that was able to take

    Delegate<uint, const ItemContainer&> on_item_changed;

private:
    List<ItemContainer> items_;

    void distribute_item_add(Map<uint, uint>& modifications, const ItemContainer& item, uint& rest);
    void distribute_item_take(Map<uint, uint>& modifications, const ItemContainer& item, uint& count);
};
