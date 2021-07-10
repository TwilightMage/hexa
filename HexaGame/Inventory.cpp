#include "Inventory.h"

#include "Database/items/ItemInfo.h"
#include "Engine/Map.h"

Inventory::Inventory(uint size)
    : items_(List<ItemContainer>(size))
{
}

const ItemContainer& Inventory::get_item(uint index) const
{
    return items_[index];
}

void Inventory::set_item(uint index, const ItemContainer& item)
{
    auto& old_item = items_[index];
    if (old_item != item)
    {
        const auto new_item = item.is_empty() ? ItemContainer() : item;
        old_item = new_item;
        on_item_changed(index, new_item);
    }
}

bool Inventory::add_item(const ItemContainer& item)
{
    if (item.is_empty()) return false;
    
    Map<uint, uint> modifications;
    uint count;
    distribute_item_add(modifications, item, count);

    if (count == 0)
    {
        for (const auto& modification : modifications)
        {
            ItemContainer item_in_slot = item;
            item_in_slot.count = items_[modification.key].count + modification.value;
            items_[modification.key] = item_in_slot;
            on_item_changed(modification.key, item_in_slot);
        }

        return true;
    }

    return false;
}

uint Inventory::add_item_force(const ItemContainer& item)
{
    if (item.is_empty()) return 0;
    
    Map<uint, uint> modifications;
    uint count;
    distribute_item_add(modifications, item, count);

    for (const auto& modification : modifications)
    {
        ItemContainer item_in_slot = item;
        item_in_slot.count = items_[modification.key].count + modification.value;
        items_[modification.key] = item_in_slot;
        on_item_changed(modification.key, item_in_slot);
    }

    return count;
}

bool Inventory::take_item(const ItemContainer& item)
{
    if (item.is_empty()) return false;
    
    Map<uint, uint> modifications;
    uint required;
    distribute_item_take(modifications, item, required);

    if (required == 0)
    {
        for (const auto& modification : modifications)
        {
            items_[modification.key].count -= modification.value;
            on_item_changed(modification.key, items_[modification.key]);
        }

        return true;
    }

    return false;
}

uint Inventory::take_item_force(const ItemContainer& item)
{
    if (item.is_empty()) return 0;
    
    Map<uint, uint> modifications;
    uint required;
    distribute_item_take(modifications, item, required);

    for (const auto& modification : modifications)
    {
        items_[modification.key].count -= modification.value;
        on_item_changed(modification.key, items_[modification.key]);
    }

    return item.count - required;
}

void Inventory::distribute_item_add(Map<uint, uint>& modifications, const ItemContainer& item, uint& rest)
{
    rest = item.count;
    for (uint i = 0; i < items_.length() && rest > 0; i++)
    {
        if (items_[i].is_empty() || items_[i].compare(item) && items_[i].item->stack_size != items_[i].count)
        {
            const auto c = Math::min(item.item->stack_size - items_[i].count, rest);
            modifications[i] = c;
            rest -= c;
        }
    }
}

void Inventory::distribute_item_take(Map<uint, uint>& modifications, const ItemContainer& item, uint& count)
{
    count = 0;
    for (uint i = 0; i < items_.length(); i++)
    {
        if (items_[i].compare(item))
        {
            if (items_[i].count < item.count - count)
            {
                modifications[i] = items_[i].count;
                count += items_[i].count;
            }
            else
            {
                modifications[i] = item.count - count;
                count = item.count;
                break;
            }
        }
    }
}
