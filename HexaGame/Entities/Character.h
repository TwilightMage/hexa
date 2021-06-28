#pragma once

#include "Engine/AnimatedField.h"
#include "Engine/AnimationSlot.h"
#include "Engine/Entity.h"
#include "Engine/ITickable.h"
#include "HexaGame/Direction.h"
#include "HexaGame/PathConfig.h"
#include "HexaGame/TileIndex.h"

class CharacterInventory;
class Animation;
class WorldPath;
class AnimatorComponent;
class HexaWorld;
class CharacterController;

class EXPORT Character : public Entity, public ITickable, public Animated
{
    friend CharacterController;
    friend HexaWorld;

public:
    Character();

    void on_start() override;
    void tick(float delta_time) override;
    
    bool go_to(const TileIndex& tile_position);

    void kill();
    
    void rotate_to(Direction direction);
    void rotate_to_tile(const TileIndex& tile_index);
    void undo_rotate_to_tile();
    
    virtual void on_character_possesed();
    virtual void on_character_un_possesed();

    virtual float get_reach_distance() const;

    TileIndex get_tile_position() const;

    FORCEINLINE const Shared<CharacterInventory>& get_inventory() const { return inventory_; }
    FORCEINLINE const Shared<AnimatorComponent>& get_animator() const { return animator_; }

    PathConfig get_path_config(const TileIndex& to) const;

    Delegate<const Shared<Character>&> on_tile_position_changed;
    Delegate<const Shared<Character>&, const Shared<WorldPath>&, const Shared<WorldPath>&> on_path_changed;

protected:
    void generate_components() override;

    void modify_matrix_params(Vector3& position, Quaternion& rotation, Vector3& scale) override;

    virtual void modify_path_config(PathConfig& config) const;

    AnimationSlot step_animation;

    uint declared_height = 1;
    uint initial_inventory_size = 40;
    float basic_reach_distance = 1.0f;
    
private:
    void set_tile_position(const TileIndex& tile_position);
    void start_next_path_segment();
    void path_segment_finished();

    void update_target_rotation();

    void parent_chunk_changed(const ChunkIndex& sender, const TileIndex& world_tile);

    Vector3 anim_offset_ = Vector3::zero();
    Vector3 anim_scale_ = Vector3::one();
    
    TileIndex tile_position_;
    Direction looking_direction_;
    TileIndex looking_to_tile_;
    Quaternion target_rotation_;
    bool has_looking_to_tile_ = false;

    Shared<WorldPath> current_path_;
    uint path_segment_index_ = 0;
    Shared<WorldPath> new_path_to_use_;
    bool is_in_path_segment_ = false;

    float move_x, move_y;
    
    uint camera_travel_distance_ = 10;

    Weak<CharacterController> controller_;

    Shared<CharacterInventory> inventory_;

    Shared<AnimatorComponent> animator_;

public:
    ANIMATED_FIELD(float, scale_x, 1,
        [this]() -> float{ return anim_scale_.x; },
        [this](float val) -> void
        {
            anim_scale_.x = val;
            mark_matrix_dirty();
        }
    );
    ANIMATED_FIELD(float, scale_y, 1,
        [this]() -> float{ return anim_scale_.y; },
        [this](float val) -> void
        {
            anim_scale_.y = val;
            mark_matrix_dirty();
        }
    );
    ANIMATED_FIELD(float, scale_z, 1,
        [this]() -> float{ return anim_scale_.z; },
        [this](float val) -> void
        {
            anim_scale_.z = val;
            mark_matrix_dirty();
        }
    );
    ANIMATED_FIELD(float, offset_x, 0,
        [this]() -> float{ return anim_offset_.x; },
        [this](float val) -> void
        {
            anim_offset_.x = val;
            mark_matrix_dirty();
        }
    );
    ANIMATED_FIELD(float, offset_y, 0,
        [this]() -> float{ return anim_offset_.y; },
        [this](float val) -> void
        {
            anim_offset_.y = val;
            mark_matrix_dirty();
        }
    );
    ANIMATED_FIELD(float, offset_z, 0,
        [this]() -> float{ return anim_offset_.z; },
        [this](float val) -> void
        {
            anim_offset_.z = val;
            mark_matrix_dirty();
        }
    );
    ANIMATED_FIELD(float, move_x, 0,
        [this]() -> float{ return move_x; },
        [this](float val) -> void { move_x = val; }
    );
    ANIMATED_FIELD(float, move_y, 0,
        [this]() -> float{ return move_y; },
        [this](float val) -> void { move_y = val; }
    );
};
