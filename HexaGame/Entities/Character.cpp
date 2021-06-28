#include "Character.h"

#include "Engine/AnimatorComponent.h"
#include "HexaGame/CharacterInventory.h"
#include "HexaGame/HexaMath.h"
#include "HexaGame/Tiles.h"
#include "HexaGame/WorldChunk.h"
#include "HexaGame/WorldPath.h"
#include "HexaGame/Worlds/HexaWorld.h"

Character::Character()
    : Entity()
    , inventory_(MakeShared<CharacterInventory>(initial_inventory_size))
{
}

void Character::on_start()
{
    if (auto world = cast<HexaWorld>(get_world()))
    {
        world->get_chunk(tile_position_.get_chunk())->on_tile_change.bind(this, &Character::parent_chunk_changed);
    }
}

void Character::tick(float delta_time)
{
    set_rotation(Quaternion::slerp(get_rotation(), target_rotation_, 0.1f));
    
    if (is_in_path_segment_)
    {
        auto& path_segment = current_path_->segments[path_segment_index_];
        set_position(Math::lerp(path_segment.from.to_vector(), path_segment.to.to_vector(), move_x) + Vector3(0, 0, HexaMath::tile_height * move_y));
    }
}

bool Character::go_to(const TileIndex& tile_position)
{
    if (auto world = cast<HexaWorld>(get_world()))
    {
        if (const auto path = world->FindPath(get_path_config(tile_position)))
        {
            new_path_to_use_ = path;
            if (!is_in_path_segment_)
            {
                start_next_path_segment();
            }
            return true;
        }
    }

    return false;
}

void Character::kill()
{
    destroy();
}

void Character::rotate_to(Direction direction)
{
    looking_direction_ = direction;
    update_target_rotation();
}

void Character::rotate_to_tile(const TileIndex& tile_index)
{
    if (has_looking_to_tile_ && looking_to_tile_ == tile_index) return;
    
    looking_to_tile_ = tile_index;
    has_looking_to_tile_ = true;
    update_target_rotation();
}

void Character::undo_rotate_to_tile()
{
    has_looking_to_tile_ = false;
    update_target_rotation();
}

void Character::on_character_possesed()
{
}

void Character::on_character_un_possesed()
{
}

float Character::get_reach_distance() const
{
    return basic_reach_distance;
}

TileIndex Character::get_tile_position() const
{
    if (is_in_path_segment_)
    {
        return current_path_->segments[path_segment_index_].to;
    }
    return tile_position_;
}

PathConfig Character::get_path_config(const TileIndex& to) const
{
    PathConfig config;
    
    modify_path_config(config);

    config.from = get_tile_position();
    config.to = to;
    config.agent_height = declared_height;

    return config;
}

void Character::generate_components()
{
    animator_ = MakeShared<AnimatorComponent>();
    add_component(animator_);
}

void Character::modify_matrix_params(Vector3& position, Quaternion& rotation, Vector3& scale)
{
    position += anim_offset_;
    scale *= anim_scale_;
}

void Character::modify_path_config(PathConfig& config) const
{
    
}

void Character::set_tile_position(const TileIndex& tile_position)
{
    const auto old_chunk_id = tile_position_.get_chunk();
    const auto new_chunk_id = tile_position.get_chunk();
    if (new_chunk_id != old_chunk_id)
    {
        if (auto world = cast<HexaWorld>(get_world()))
        {
            world->get_chunk(old_chunk_id)->on_tile_change.unbind(this, &Character::parent_chunk_changed);
            world->get_chunk(new_chunk_id)->on_tile_change.bind(this, &Character::parent_chunk_changed);
        }
    }
    
    tile_position_ = tile_position;
    set_position(tile_position_.to_vector());
    on_tile_position_changed(cast<Character>(shared_from_this()));
}

void Character::start_next_path_segment()
{
    if (!current_path_ && !new_path_to_use_) return;

    is_in_path_segment_ = true;
    
    if (new_path_to_use_)
    {
        const auto old_path = current_path_;
        current_path_ = new_path_to_use_;
        path_segment_index_ = 0;
        new_path_to_use_ = nullptr;

        on_path_changed(cast<Character>(shared_from_this()), old_path, current_path_);
    }
    else
    {
        path_segment_index_++;
    }
    
    auto& segment = current_path_->segments[path_segment_index_];
    if (TileIndex::distance_xy(segment.from, segment.to) == 1)
    {
        const TileIndex offset = segment.to - segment.from;

        TileIndex direction_offset = offset;
        direction_offset.z = 0;
        TileSide step_direction = TileIndex::offset_to_side(direction_offset);
        rotate_to((Direction)Math::log2((float)step_direction));
        
        if (segment.from.z > segment.to.z) // jump down
        {
            /*if (auto world = get_world())
            {
                world->delay(1, [this]()
                {
                    path_segment_finished();
                });
            }*/
            auto anim_handle = animator_->play_animation(step_animation.animation, 0, 1, step_animation.reverse);
            anim_handle.on_end()->bind(this, &Character::path_segment_finished);
        }
        else if (segment.from.z == segment.to.z) // step
        {
            auto anim_handle = animator_->play_animation(step_animation.animation, 0, 1, step_animation.reverse);
            anim_handle.on_end()->bind(this, &Character::path_segment_finished);
        }
        else // climb
        {
            /*if (auto world = get_world())
            {
                world->delay(1, [this]()
                {
                    path_segment_finished();
                });
            }*/
            auto anim_handle = animator_->play_animation(step_animation.animation, 0, 1, step_animation.reverse);
            anim_handle.on_end()->bind(this, &Character::path_segment_finished);
        }
    }
}

void Character::path_segment_finished()
{
    is_in_path_segment_ = false;

    set_tile_position(current_path_->segments[path_segment_index_].to);

    if (path_segment_index_ == current_path_->segments.length() - 1 && !new_path_to_use_)
    {
        const auto old_path = current_path_;
        current_path_ = nullptr;
        path_segment_index_ = 0;
        if (looking_to_tile_.x == tile_position_.x && looking_to_tile_.y == tile_position_.y)
        {
            looking_to_tile_ = tile_position_ + TileIndex::offset_from_side((TileSide)(1 << (int)looking_direction_));
        }
        update_target_rotation();

        on_path_changed(cast<Character>(shared_from_this()), old_path, nullptr);
        
        return;
    }

    start_next_path_segment();
}

void Character::update_target_rotation()
{
    if (has_looking_to_tile_ && looking_to_tile_ != tile_position_ && !is_in_path_segment_)
    {
        auto to = looking_to_tile_.to_vector();
        auto from = tile_position_.to_vector();
        to.z = 0;
        from.z = 0;
        const auto delta = (to - from);
        target_rotation_ = Quaternion::look_at(delta.normalized());
    }
    else
    {
        target_rotation_ = Quaternion(Vector3(0, 0, static_cast<float>(looking_direction_) * 60));
    }
}

void Character::parent_chunk_changed(const ChunkIndex& sender, const TileIndex& world_tile)
{
    if (auto world = cast<HexaWorld>(get_world()))
    {
        if (world_tile == get_tile_position().offset(TileSide::Down))
        {
            for (int i = 1; i < get_tile_position().z; i++)
            {
                if (world->get_tile_id(get_tile_position().offset(0, 0, -i)) != Tiles::air)
                {
                    set_tile_position(get_tile_position().offset(0, 0, -i + 1));
                    return;
                }
            }

            kill();
        }
    }
}
