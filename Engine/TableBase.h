#pragma once

#include "Compound.h"
#include "ModuleAssetID.h"
#include "Name.h"
#include "Object.h"
#include "Pointers.h"

class Game;
class Module;

class EXPORT TableBase : public Object
{
    friend Game;
    
public:
    virtual ~TableBase(){ }
    
    TableBase(const String& name);

    virtual void add_record_compound(const ModuleAssetID& key, const Compound::Object& new_record, bool force_replace = false) = 0;
    virtual void add_record_compounds(const Shared<Module>& owning_module, const Compound::Object& record_compound, bool force_replace = false) = 0;

private:
    virtual void post_load() {}
    virtual void init_assets() {}
    virtual void clear() {}
};
