#pragma once

#include "HexaGame/Entities/Character.h"

class Slime : public Character
{
public:
    Slime();

protected:
    void on_start() override;

    void tick(float delta_time) override;
};
