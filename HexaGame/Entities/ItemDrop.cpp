#include "ItemDrop.h"

#include "Engine/Logger.h"
#include "Engine/Physics/ConcaveMeshCollision.h"
#include "HexaGame/HexaCollisionMaskBits.h"
#include "HexaGame/ItemInfo.h"

ItemDrop::ItemDrop(const ItemContainer& item)
    : Entity()
    , item_(item)
{
    name = item.item ? String::format("%i %s drop", item.count, item.item->name.c()) : "Empty drop";
}

void ItemDrop::on_start()
{
    make_body_kinematic();
    set_collision_mask(HexaCollisionMaskBits::ITEM);
    
    if (!item_.is_empty())
    {
        set_mesh(item_.item->mesh);
        get_material_instance()->set_param_value("texture", item_.item->mesh_texture);
        set_collision(MakeShared<ConcaveMeshCollision>(item_.item->mesh));
    }
    else
    {
        print_warning("Item Drop", "Empty item dropped!");
    }
}
