#include "InventoryItem.h"

#include "ItemLogic.h"

InventoryItem::InventoryItem(const Name& key, const Compound::Object& data, const Shared<ItemLogic>& logic, uint count)  
{
    count_ = count;
    if (count > 0)
    {
        key_ = key;
        data_ = data;
        logic_ = logic;
    }
    else
    {
        key_ = "";
        data_ = Compound::Object();
        logic_ = nullptr;
    }
}

void InventoryItem::tick(float delta_time)
{
    if (count_ > 0) logic_->tick(shared_from_this(), delta_time);
}

void InventoryItem::on_pick(const Shared<Character>& character)
{
    if (count_ > 0) logic_->on_pick(shared_from_this(), character);
}

void InventoryItem::on_drop(const Shared<Character>& character, const Vector3& world_position)
{
    if (count_ > 0) logic_->on_drop(shared_from_this(), character, world_position);
}

void InventoryItem::set_count(uint new_count)
{
    if (count_ == 0) return;

    count_ = new_count;
    
    if (count_ == 0)
    {
        key_ = "";
        data_ = Compound::Object();
        logic_ = nullptr;
    }
}

void InventoryItem::change_count(int count_delta)
{
    set_count(Math::max((int)count_ + count_delta, 0));
}

Shared<InventoryItem> InventoryItem::split(uint count)
{
    if (count_ > 0 && count > 0)
    {
        Shared<InventoryItem> new_item = MakeShared<InventoryItem>(key_, data_, logic_, count);
        change_count(-(int)count);
        return new_item;
    }

    return nullptr;
}
