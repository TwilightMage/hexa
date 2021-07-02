#pragma once

#include "BasicTypes.h"
#include "Matrix4x4.h"
#include "RendererInstance.h"
#include "RendererParameters.h"

class EXPORT Renderer3DInstance : public RendererInstance
{
public:
    void set_model(const Matrix4x4& value) const;
    void set_texture(const Shared<Texture>& value) const;

protected:
    void register_direct_parameters() override;

private:
    Shared<RendererParameter<Matrix4x4>> model;
    Shared<RendererParameter<Shared<Texture>>> texture;
};
