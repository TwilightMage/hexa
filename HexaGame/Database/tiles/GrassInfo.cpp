#include "GrassInfo.h"

#include "HexaGame/Database/Items.h"

List<ItemContainer> GrassInfo::get_drops(const TileIndex& index, const Shared<HexaWorld>& world) const
{
    return { ItemContainer(Items::dirt) };
}
