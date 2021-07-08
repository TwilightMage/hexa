#pragma once

#include "BasicTypes.h"
#include "Matrix4x4.h"
#include "MaterialInstance.h"
#include "MaterialParameters.h"

class EXPORT Material3DInstance : public MaterialInstance
{
public:
    void set_model(const Matrix4x4& value) const;

protected:
    void register_direct_parameters() override;

private:
    Shared<MaterialParameter<Matrix4x4>> model;
};
