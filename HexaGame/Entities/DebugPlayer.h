#pragma once

#include "MeshEntity.h"
#include "Engine/Entity.h"
#include "Engine/KeyCode.h"
#include "Engine/Player.h"
#include "HexaGame/ChunkIndex.h"

class TextBlock;
class WorldChunkObserver;
class Camera;

class EXPORT DebugPlayer : public Player
{
public:
    void on_start() override;
    
    void key_down(KeyCode key) override;
    void key_up(KeyCode key) override;
    void mouse_button_down(int button) override;

    void on_possess() override;

    void on_tick(float delta_time) override;
    
private:
    float move_forward_ = 0;
    float move_right_ = 0;
    float move_up_ = 0;

    Shared<MeshEntity> arrows_;

    Shared<WorldChunkObserver> observer_;
    ChunkIndex old_chunk_ = ChunkIndex(0, 0);
    uint load_distance_ = 3;
    Shared<TextBlock> debug_text_;
};
