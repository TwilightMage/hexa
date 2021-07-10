#pragma once

#include "BasicTypes.h"
#include "Path.h"

class Game;
class World;

namespace SoLoud
{
    class Wav;
}

class EXPORT Audio
{
    friend World;
    friend Game;
    
public:
    static Shared<Audio> load(const Path& path);

    void set_looped(bool looped);
    bool is_looped() const;

    float get_duration() const;

private:
    Shared<SoLoud::Wav> sample_;
    bool looped_;
};
