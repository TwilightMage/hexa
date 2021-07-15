#include "Database.h"

#include "Game.h"

template <class T>
Database<T>::Database(const String& name)
    : DatabaseBase(name)
{
}

template <class T>
template<class SubT>
Shared<const SubT> Database<T>::add(SubT* record)
{
    if (Game::is_loading_stage())
    {
        auto& data_record = data_[record->key];
        if (!data_record)
        {
            data_record = Shared<SubT>(record);

            return cast<SubT>(data_record);
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
Shared<const T> Database<T>::get(const Name& key) const
{
    if (auto found = data_.find(key))
    {
        return *found;
    }
        
    return nullptr;
}

template <class T>
const Map<Name, Shared<T>>& Database<T>::records() const
{
    return data_;
}
