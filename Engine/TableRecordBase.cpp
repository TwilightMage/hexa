#include "TableRecordBase.h"

void TableRecordBase::convert_to(Compound::Object& to) const
{
    to["id"] = id.to_string();
}

void TableRecordBase::convert_from(const Compound::Object& from)
{
    id = ModuleAssetID(from.get_string("id"));
}
