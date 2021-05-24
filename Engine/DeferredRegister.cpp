#include "DeferredRegister.h"

#include "Database.h"

template <class C>
DeferredRegister<C>::DeferredRegister(C(* record_provider)())
    : record_provider_(record_provider)
    , registration_performed_(false)
{
}

template <class C>
DeferredRegister<C>::DeferredRegister()
{
}

template <class C>
void DeferredRegister<C>::perform_registration(Database<C>* database)
{
    if (!registration_performed_)
    {
        auto rec = record_provider_();
        cached_value_ = database->add(rec);
        registration_performed_ = true;
    }
}

template <class C>
const Shared<const C>& DeferredRegister<C>::get() const
{
    return cached_value_;
}
