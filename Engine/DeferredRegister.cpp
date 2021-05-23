#include "DeferredRegister.h"

#include "Database.h"

template <class C>
DeferredRegister<C>::DeferredRegister(uint, Shared<Database<C>>&(* db_provider)(), C(* record_provider)())
    : db_provider_(db_provider)
    , record_provider_(record_provider)
    , registration_performed_(false)
{
}

template <class C>
void DeferredRegister<C>::perform_registration()
{
    if (!registration_performed_)
    {
        cached_value_ = db_provider_()->add(record_provider_());
        registration_performed_ = true;
    }
}

template <class C>
const Shared<const C>& DeferredRegister<C>::get() const
{
    return cached_value_;
}