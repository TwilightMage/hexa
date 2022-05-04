#pragma once
#include "Engine/Compound.h"
#include "Engine/Name.h"
#include "Engine/Pointers.h"
#include "Engine/Vector3.h"

class Character;
class ItemLogic;

class EXPORT InventoryItem : public std::enable_shared_from_this<InventoryItem>
{
public:
    InventoryItem(const Name& key, const Compound::Object& data, const Shared<ItemLogic>& logic, uint count);

    void tick(float delta_time);
    void on_pick(const Shared<Character>& character);
    void on_drop(const Shared<Character>& character, const Vector3& world_position);

    FORCEINLINE const Name& get_key() const { return key_; }
    FORCEINLINE const Compound::Object& get_data() const { return data_; }
    FORCEINLINE const Shared<ItemLogic>& get_logic() const { return logic_; }
    FORCEINLINE uint get_count() const { return count_; }

    void set_count(uint new_count);
    void change_count(int count_delta);

    FORCEINLINE bool is_empty() const { return count_ == 0; }

    Shared<InventoryItem> split(uint count);

private:
    Name key_; 
    Compound::Object data_;
    Shared<ItemLogic> logic_;
    uint count_;
};
