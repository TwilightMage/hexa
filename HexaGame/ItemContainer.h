#pragma once

#include "Engine/BasicTypes.h"

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
    
    Shared<const ItemInfo> item;
    uint count;

    FORCEINLINE bool is_empty() const { return item == nullptr || count == 0; }
};
