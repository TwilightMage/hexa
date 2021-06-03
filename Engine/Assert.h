#pragma once

#include "Utils.h"

#define Assert(condition)\
if (!(condition))\
{\
    _wassert(_CRT_WIDE(#condition), _CRT_WIDE(__FILE__), (unsigned)(__LINE__));;\
    print_error("assert", "failed %s at\n%s", #condition, Utils::get_callstack_string().c());\
}