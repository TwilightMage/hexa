#pragma once

#include "Engine/Entity.h"
#include "HexaGame/ItemContainer.h"

class ItemInfo;

class ItemDrop : public Entity
{
public:
    ItemDrop(const ItemContainer& item);

    void on_start() override;

    FORCEINLINE const ItemContainer& get_item() const { return item_; }

protected:
    bool is_rigid_body() const override { return true; }

private:
    ItemContainer item_;
};
