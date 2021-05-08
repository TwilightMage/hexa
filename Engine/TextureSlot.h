#pragma once

#include "BasicTypes.h"
#include "framework.h"

class Texture;

EXTERN class EXPORT TextureSlot
{
public:
    TextureSlot();
    TextureSlot(const TextureSlot& rhs);
    TextureSlot(const Shared<Texture>& rhs);
    explicit TextureSlot(bool state);
    TextureSlot(const Shared<Texture>& texture, bool state);
    ~TextureSlot();

    void activate();
    void deactivate();
    
    TextureSlot& operator=(const TextureSlot& rhs);
    const Shared<Texture>& operator->() const;
    const Shared<Texture>& operator*() const;

private:
    Shared<Texture> texture_;
    bool is_active_;
};
