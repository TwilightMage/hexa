#pragma once

#include "DatabaseBase.h"
#include "Map.h"
#include "Pointers.h"
#include "String.h"

template<class T>
class Database : public DatabaseBase
{
public:
    explicit Database(const String& name);

    template<class SubT>
    Shared<const SubT> add(SubT* record);
    Shared<const T> get(const String& key) const;
    const Map<String, Shared<T>>& records() const;

private:
    Map<String, Shared<T>> data_;
};

#include "Database.cpp"