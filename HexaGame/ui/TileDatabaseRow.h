#pragma once

#include "Engine/String.h"
#include "Engine/Texture.h"
#include "Engine/ui/UIElement.h"

EXTERN class EXPORT TileDatabaseRow : public UIElement
{
public:
    TileDatabaseRow(const String& key, const Shared<Texture>& texture);

protected:
    void on_construct() override;

private:
    String key_;
    Shared<Texture> texture_;
};
