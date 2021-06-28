#include "Toolbar.h"

#include "Engine/SpriteFont.h"
#include "Engine/Texture.h"
#include "Engine/ui/Image.h"
#include "Engine/ui/TextBlock.h"
#include "HexaGame/CharacterInventory.h"
#include "HexaGame/ItemInfo.h"
#include "HexaGame/Paths.h"

void Toolbar::on_construct()
{    
    auto background = MakeShared<Image>(Texture::load_png(RESOURCES_HEXA_TEXTURES_UI + "toolbar.png"));
    background->set_size(Vector2(373, 40));
    background->set_position(Vector2(1, 1));
    background->set_mouse_detection(false);
    add_child(background);

    for (uint i = 0; i < 10; i++)
    {
        slot_infos_[i].rect = Rect(4 + 37 * i, 4, 34, 34);
        
        auto icon = MakeShared<Image>();
        icon->set_size(Vector2(32, 32));
        icon->set_position(Vector2(5 + 37 * static_cast<float>(i), 5));
        icon->set_z(KINDA_SMALL_NUMBER * 2);
        icon->set_mouse_detection(false);
        slot_infos_[i].icon = icon;

        auto label = MakeShared<TextBlock>(String::make((i + 1) % 10));
        const auto label_size = label->get_font()->measure_string(label->get_text(), label->get_font_size());
        label->set_position(Vector2(5 + 37 * static_cast<float>(i) + (37 - label_size) / 2, 35));
        label->set_z(KINDA_SMALL_NUMBER * 2);
        label->set_mouse_detection(false);
        add_child(label);
    }

    selection_frame_ = MakeShared<Image>(Texture::load_png(RESOURCES_HEXA_TEXTURES_UI + "toolbar_selection.png"));
    selection_frame_->set_size(Vector2(42, 42));
    selection_frame_->set_z(KINDA_SMALL_NUMBER);
    selection_frame_->set_mouse_detection(false);
    add_child(selection_frame_);

    set_size(Vector2(373 + 2, 40 + 2));

    update_geometry();
}

void Toolbar::bind(const Shared<CharacterInventory>& inventory)
{
    if (inventory == inventory_) return;

    if (inventory_)
    {
        inventory_->on_hotbar_select.unbind(this, &Toolbar::selection_change);
        inventory_->on_item_changed.unbind(this, &Toolbar::item_changed);
    }

    if (inventory)
    {
        inventory->on_hotbar_select.bind(this, &Toolbar::selection_change);
        inventory->on_item_changed.bind(this, &Toolbar::item_changed);
    }

    inventory_ = inventory;

    if (is_started_construction())
    {
        update_geometry();
    }
}

void Toolbar::on_parent_size_changed()
{
    if (is_started_construction())
    {
        if (auto parent = get_parent())
        {
            set_position(Vector2(5, 5));
        }
    }
}

void Toolbar::on_press(const Vector2& point)
{
    auto pt = point - Vector2(1, 1);
    if (pt.x >= 0 && pt.x < get_size().x - 2 && pt.y >= 0 && pt.y < get_size().y - 2)
    {
        const auto slot = slot_infos_.IndexOf([&](const SlotInfo& info)->bool
        {
            return info.rect.contains(point);
        });
        if (slot >= 0)
        {
            inventory_->set_selected_hotbar(slot);
        }
    }
}

void Toolbar::update_geometry()
{
    selection_change(inventory_ ? inventory_->get_selected_hotbar() : 0);

    for (uint i = 0; i < 10; i++)
    {
        item_changed(i, inventory_ ? inventory_->get_item(i) : ItemContainer());
    }
}

void Toolbar::item_changed(uint index, const ItemContainer& item)
{
    if (index < 10)
    {
        if (item.is_empty())
        {
            slot_infos_[index].icon->remove_from_parent();
        }
        else
        {
            add_child(slot_infos_[index].icon);
            slot_infos_[index].icon->use_texture(item.item->icon);
        }
    }
}

void Toolbar::selection_change(uint selection)
{
    selection_frame_->set_position(Vector2(37 * (float)selection, 0));
}
