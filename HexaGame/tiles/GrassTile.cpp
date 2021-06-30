#include "GrassTile.h"

#include "HexaGame/Items.h"

List<ItemContainer> GrassTile::get_drops(const TileIndex& index, const Shared<World>& world) const
{
    return { ItemContainer(Items::dirt) };
}
