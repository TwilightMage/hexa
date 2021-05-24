#pragma once

#include "BasicTypes.h"

template<typename T>
class Database;


template<class C>
class DeferredRegister
{
public:
    DeferredRegister(C(* record_provider)());
    DeferredRegister();
    
    void perform_registration(Database<C>* database);

    const Shared<const C>& get() const;

private:    
    C(* record_provider_)();
    Shared<const C> cached_value_;
    bool registration_performed_;
};

#include "DeferredRegister.cpp"