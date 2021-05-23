#pragma once

#include "BasicTypes.h"

#define USE_DATABASE(type, path)\
    inline static auto db_provider = []()->Shared<Database<type>>&{ return path; };

#define BEGIN_RECORDS()\
    public:\
    inline static uint begin = __COUNTER__;

#define END_RECORDS()\
    inline static uint end = __COUNTER__;

#define RECORD(type, name, ...)\
    inline static DeferredRegister<type> name = DeferredRegister<type>(__COUNTER__, db_provider, [](){return type{ #name, __VA_ARGS__};});