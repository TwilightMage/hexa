#include "GrassInfo.h"

#include "HexaGame/Items.h"

List<ItemContainer> GrassInfo::get_drops(const TileIndex& index, const Shared<HexaWorld>& world) const
{
    return { ItemContainer(Items::dirt) };
}
