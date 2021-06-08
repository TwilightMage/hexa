#pragma once

#include "Direction.h"
#include "TileIndex.h"
#include "Engine/Entity.h"
#include "Engine/ITickable.h"

class HexaWorld;
class CharacterController;

EXTERN class EXPORT Character : public Entity, public ITickable
{
    friend CharacterController;
    friend HexaWorld;

public:
    void tick(float delta_time) override;
    
    void go_to(const TileIndex& tile_position);

    void rotate_to(Direction direction);
    void rotate_to_tile(const TileIndex& tile_index);
    void undo_rotate_to_tile();
    
    virtual void on_character_possesed();
    virtual void on_character_un_possesed();

    FORCEINLINE const TileIndex& get_tile_position() const;

private:
    void set_tile_position(const TileIndex& tile_position);

    float get_desired_rotation_angle() const;
    
    TileIndex tile_position_;
    Direction looking_direction_;
    TileIndex looking_to_tile_;
    bool has_looking_to_tile_ = false;
    
    uint camera_travel_distance_ = 10;

    Weak<CharacterController> controller_;
};
