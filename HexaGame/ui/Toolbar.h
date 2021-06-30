#pragma once
#include "Engine/BasicTypes.h"
#include "Engine/ui/UIElement.h"
#include "HexaGame/ItemContainer.h"

class TextBlock;
class Image;
class CharacterInventory;
class Texture;

class Toolbar : public UIElement
{
public:
    struct SlotInfo
    {
        Shared<Image> icon;
        Shared<TextBlock> count_label;
        Rect rect;
    };
    
    void on_construct() override;

    void bind(const Shared<CharacterInventory>& inventory);

protected:
    void on_parent_size_changed() override;

    void on_press(const Vector2& point) override;

private:
    List<SlotInfo> slot_infos_ = List<SlotInfo>(10);
    Shared<Image> selection_frame_;

    Shared<CharacterInventory> inventory_;

    void update_geometry();

    void item_changed(uint index, const ItemContainer& item);
    void selection_change(uint selection);
};
