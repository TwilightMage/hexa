#pragma once

#include "BasicTypes.h"

class Texture;

class TextureSlot
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
