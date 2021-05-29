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
Shared<const T> Database<T>::get(const String& key) const
{
    auto found = data_.find(key);
    if (found != data_.end())
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
