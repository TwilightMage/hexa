#include "Character.h"

void Character::tick(float delta_time)
{
    set_rotation(Quaternion(Vector3(0, 0, Math::lerp(get_rotation().yaw(), get_desired_rotation_angle(), 0.1f))));
}

void Character::go_to(const TileIndex& tile_position)
{
}

void Character::rotate_to(Direction direction)
{
    looking_direction_ = direction;
}

void Character::rotate_to_tile(const TileIndex& tile_index)
{
    looking_to_tile_ = tile_index;
    has_looking_to_tile_ = true;
}

void Character::undo_rotate_to_tile()
{
    has_looking_to_tile_ = false;
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

float Character::get_desired_rotation_angle() const
{
    if (has_looking_to_tile_ && looking_to_tile_ != tile_position_)
    {
        auto from = looking_to_tile_.to_vector();
        auto to = tile_position_.to_vector();
        from.z = 0;
        to.z = 0;
        auto delta = (from - to);
        return Quaternion::look_at(delta.normalized()).yaw();
    }
    else
    {
        return static_cast<float>(looking_direction_) * 60;
    }
}
