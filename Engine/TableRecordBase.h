#pragma once

#include "Compound.h"
#include "IConvertible.h"
#include "ModuleAssetID.h"

class EXPORT TableRecordBase : public IConvertible<Compound::Object>
{
public:
    void convert_to(Compound::Object& to) const override;
    void convert_from(const Compound::Object& from) override;
    
    virtual void post_load() {}
    virtual void init_assets() {}

    FORCEINLINE const ModuleAssetID& get_id() const { return id; }
    
protected:
    ModuleAssetID id;
};
