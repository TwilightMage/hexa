#pragma once

#include <map>


#include "DatabaseBase.h"
#include "DeferredRegister.h"
#include "String.h"

template<class T>
class Database : public DatabaseBase
{
public:
    explicit Database(const String& name);
    
    Shared<const T> add(const T& record);
    Shared<const T> get(const String& key) const;
    const std::map<String, Shared<T>>& records() const;

    template<class C>
    void register_entries();

private:
    std::map<String, Shared<T>> data_;
};

#include "Database.cpp"