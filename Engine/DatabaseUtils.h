#pragma once

#include "BasicTypes.h"

#define BEGIN_RECORDS(type)\
    public:\
    inline static DeferredRegister<type> begin = DeferredRegister<type>();

#define END_RECORDS(type)\
    inline static DeferredRegister<type> end = DeferredRegister<type>();

#define RECORD(type, name, ...)\
    inline static DeferredRegister<type> name = DeferredRegister<type>([](){return type{ #name, __VA_ARGS__};});