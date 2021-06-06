#include "Character.h"

void Character::tick(float delta_time)
{
}

void Character::go_to(const TileIndex& tile_position)
{
}

void Character::on_character_possesed()
{
}

void Character::on_character_un_possesed()
{
}

const TileIndex& Character::get_tile_position() const
{
    return tile_position_;
}

void Character::set_tile_position(const TileIndex& tile_position)
{
    tile_position_ = tile_position;
    set_position(tile_position_.to_vector());
}
