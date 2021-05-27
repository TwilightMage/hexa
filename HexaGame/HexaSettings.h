#pragma once

#include "Engine/Settings.h"

EXTERN class EXPORT HexaSettings : public Settings
{
public:
    FORCEINLINE uint get_max_threads() const { return max_threads_; }

private:
    uint max_threads_ = 10;
};
