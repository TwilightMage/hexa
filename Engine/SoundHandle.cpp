#include "SoundHandle.h"

#include <soloud/soloud.h>

SoundHandle::SoundHandle()
    : handle_(0)
    , engine_(nullptr)
{
}

void SoundHandle::stop()
{
    if (engine_ && handle_)
    {
        engine_->stop(handle_);
        handle_ = 0;
        engine_ = nullptr;
    }
}

void SoundHandle::set_pause(bool pause) const
{
    if (engine_ && handle_)
    {
        engine_->setPause(handle_, pause);
    }
}

void SoundHandle::set_volume(float vol) const
{
    if (engine_ && handle_)
    {
        engine_->setVolume(handle_, vol);
    }
}

float SoundHandle::get_volume() const
{
    if (engine_ && handle_)
    {
        return engine_->getVolume(handle_);
    }

    return 0.0f;
}

SoundHandle::SoundHandle(int handle, Shared<SoLoud::Soloud> engine)
    : handle_(handle)
    , engine_(engine)
{
}