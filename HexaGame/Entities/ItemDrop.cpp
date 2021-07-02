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
    use_collision_mask(HexaCollisionMaskBits::ITEM);
    
    if (!item_.is_empty())
    {
        use_mesh(item_.item->mesh);
        use_texture(item_.item->mesh_texture);
        use_collision(MakeShared<ConcaveMeshCollision>(item_.item->mesh));
    }
    else
    {
        print_warning("Item Drop", "Empty item dropped!");
    }
}
