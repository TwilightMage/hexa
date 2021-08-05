#pragma once

#include "DatabaseBase.h"
#include "Map.h"
#include "Name.h"
#include "Pointers.h"
#include "String.h"

template<class T>
class Database : public DatabaseBase
{
public:
    explicit Database(const String& name);

    template<class SubT>
    ConstPtr<SubT> add(ConstPtr<SubT> record);
    ConstPtr<T> get(const Name& key) const;
    const Map<Name, ConstPtr<T>>& records() const;

    void clear();

private:
    Map<Name, ConstPtr<T>> data_;
};

#include "Database.cpp"