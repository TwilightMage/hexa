#include "Database.h"

#include "Game.h"

template <class T>
Database<T>::Database(const String& name)
    : DatabaseBase(name)
{
}

template <class T>
Shared<const T> Database<T>::add(const T& record)
{
    if (Game::is_loading_stage())
    {
        auto& data_record = data_[record.key];
        if (!data_record)
        {
            data_record = MakeShared<T>();
            *data_record = record;

            return data_record;
        }
        else
        {
            // TODO: Print error
        }
    }
    else
    {
        // TODO: Print error
    }

    return nullptr;
}

template <class T>
Shared<const T> Database<T>::get(const String& key)
{
    if (auto& found = data_.find(key))
    {
        return found->second;
    }
        
    return nullptr;
}

template <class T>
const std::map<String, Shared<T>>& Database<T>::records() const
{
    return data_;
}

template <class T>
template <class C>
void Database<T>::register_entries()
{
    // I don't give a fuck, why "+ 8" is required, but the code doesn't work without it
    const static uint stride = sizeof(DeferredRegister<T>) + 8;
    
    const uint64 count = (reinterpret_cast<byte*>(&C::end) - reinterpret_cast<byte*>(&C::begin)) / stride - 1;
    byte* data_start = reinterpret_cast<byte*>(&C::begin) + stride;
    for (uint i = 0; i < count; i++)
    {
        reinterpret_cast<DeferredRegister<T>*>(data_start + stride * i)->perform_registration(this);
    }
}
