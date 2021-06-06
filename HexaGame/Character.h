#pragma once

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

    virtual void on_character_possesed();
    virtual void on_character_un_possesed();

    FORCEINLINE const TileIndex& get_tile_position() const;

private:
    void set_tile_position(const TileIndex& tile_position);
    
    TileIndex tile_position_ = TileIndex(0, 0, 0);

    uint camera_travel_distance_ = 10;

    Weak<CharacterController> controller_;
};
