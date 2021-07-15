﻿#pragma once

#include "BasicTypes.h"
#include "framework.h"
#include "Pointers.h"

class World;

namespace SoLoud
{
    class Soloud;
}

class EXPORT SoundHandle
{
    friend World;
    
public:
    SoundHandle();
    
    void stop();

    void set_pause(bool pause) const;
    
    void set_volume(float vol) const;
    float get_volume() const;
    
private:
    SoundHandle(int handle);
    
    int handle_ = 0;
};
