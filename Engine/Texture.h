#pragma once

#include "Color.h"
#include "ITexture.h"
#include "List.h"
#include "Path.h"

class Game;

EXTERN class EXPORT Texture : public ITexture
{
    friend Game;
    
public:
    Texture();
    Texture(uint width, uint height, List<Color> pixels);
    
    static Shared<Texture> load_png(const Path& path);

    virtual uint get_gl_id() override;
    
private:
    void usage_count_changed();
    void cleanup();
    
    List<Color> pixels_;
    uint width_ = 0;
    uint height_ = 0;
    uint usage_count_ = 0;
    uint gl_binding_ = 0;
};
