#include "Audio.h"

#include <soloud/soloud_wav.h>

#include "Assert.h"
#include "Game.h"

Shared<Audio> Audio::load(const Path& path)
{
    auto path_str = path.get_absolute_string();
    
    if (auto found = Game::instance_->audios_.find(path_str))
    {
        return *found;
    }
    
    if(Check(path.exists(), "Audio", "Audio file %s does not exist", path_str.c()))
    {
        auto result = MakeShared<Audio>();
        result->sample_ = MakeShared<SoLoud::Wav>();
        result->sample_->load(path_str.c());
        Game::instance_->audios_[path_str] = result;

        return result;
    }

    return nullptr;
    
}

void Audio::set_looped(bool looped)
{
    looped_ = looped;
    sample_->setLooping(looped);
}

bool Audio::is_looped() const
{
    return looped_;
}

float Audio::get_duration() const
{
    return sample_->getLength();
}
