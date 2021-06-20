#pragma once

#include "Engine/World.h"

class AnimationEditorUI;
class EditorPlayer;
class Animated;

class EXPORT AnimationEditorWorld : public World
{
public:
    void on_start() override;
    void open(const Shared<Animated>& animated);

private:
    Shared<EditorPlayer> player_;
    Shared<AnimationEditorUI> ui_;
};
