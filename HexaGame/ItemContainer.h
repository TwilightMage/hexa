#pragma once

#include "Engine/BasicTypes.h"
#include "Engine/Pointers.h"

class ItemInfo;

struct ItemContainer
{
    ItemContainer()
        : item(nullptr)
        , count(0)
    {
    }
    
    ItemContainer(const ItemContainer& rhs)
        : item(rhs.item)
        , count(rhs.count)
    {
    }
    
    ItemContainer(const Shared<const ItemInfo>& item)
        : item(item)
        , count(1)
    {
    }
    
    ItemContainer(const Shared<const ItemInfo>& item, uint count)
        : item(item)
        , count(count)
    {
    }

    bool operator==(const ItemContainer& rhs) const
    {
        return item == rhs.item && count == rhs.count;
    }

    bool operator!=(const ItemContainer& rhs) const
    {
        return item != rhs.item || count != rhs.count;
    }

    bool compare(const ItemContainer& rhs) const
    {
        return !is_empty() && !rhs.is_empty() && item == rhs.item;
    }
    
    Shared<const ItemInfo> item = nullptr;
    uint count = 0;

    bool is_empty() const { return item == nullptr || count == 0; }
};
