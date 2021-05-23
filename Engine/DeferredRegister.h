#pragma once

#include "BasicTypes.h"

template<typename T>
class Database;


template<class C>
class DeferredRegister
{
public:
    DeferredRegister(uint, Shared<Database<C>>&(* db_provider)(), C(* record_provider)());

    void perform_registration();

    const Shared<const C>& get() const;

private:
    Shared<Database<C>>&(* db_provider_)();
    C(* record_provider_)();
    Shared<const C> cached_value_;
    bool registration_performed_;
};

#include "DeferredRegister.cpp"