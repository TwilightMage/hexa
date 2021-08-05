#include "Database.h"

#include "Game.h"

template <class T>
Database<T>::Database(const String& name)
    : DatabaseBase(name)
{
}

template <class T>
template<class SubT>
ConstPtr<SubT> Database<T>::add(ConstPtr<SubT> record)
{
    if (Game::is_loading_stage())
    {
        auto& data_record = data_[record->key];
        if (!data_record)
        {
            data_record = record;

            return record;
        }
        else
        {
            print_error("Database", "Attempt to register record %s which already exists", record->key.to_string().c());
        }
    }
    else
    {
        print_error("Database", "Adding records allowed only during loading stage");
    }

    return nullptr;
}

template <class T>
ConstPtr<T> Database<T>::get(const Name& key) const
{
    return data_.find_or_default(key, nullptr);
}

template <class T>
const Map<Name, const T*>& Database<T>::records() const
{
    return data_;
}

template <class T>
void Database<T>::clear()
{
    if (Game::get_instance()->is_unloading_stage())
    {
        for (auto& record : data_)
        {
            delete record.value;
        }
        
        data_.clear();
    }
}
