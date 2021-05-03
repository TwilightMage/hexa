﻿#include "Texture.h"

#include "File.h"
#define STB_IMAGE_STATIC
#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

#include "Game.h"

Texture::Texture()
    : pixels_(0)
    , width_(0)
    , height_(0)
{
}

Texture::Texture(uint width, uint height, List<Color> pixels)
    : pixels_(pixels)
    , width_(width)
    , height_(height)
{
}

Shared<Texture> Texture::load_png(const Path& path)
{
    const auto found = Game::instance_->textures_.find(path.get_absolute_string());
    if (found != Game::instance_->textures_.end())
    {
        return found->second;
    }
    
    if (path.exists())
    {
        int tex_width, tex_height, tex_channels;
        const auto pixels = stbi_load(path.get_absolute_string().c(), &tex_width, &tex_height, &tex_channels, STBI_rgb_alpha);
        if (pixels)
        {
            const uint size = tex_width * tex_height * 4;
            auto result = MakeShared<Texture>();
            result->pixels_ = List(new Color[size], size);
            memcpy(result->pixels_.GetData(), pixels, size);
            stbi_image_free(pixels);
            result->width_ = tex_width;
            result->height_ = tex_height;

            Game::instance_->textures_[path.get_absolute_string()] = result;
            
            return result;
        }
        stbi_image_free(pixels);
    }

    return nullptr;
}

uint Texture::get_gl_id()
{
    return gl_binding_;
}

void Texture::usage_count_changed()
{
    if (usage_count_ > 0 && gl_binding_ == 0)
    {
        glGenTextures(1, &gl_binding_);
        glBindTexture(GL_TEXTURE_2D, gl_binding_);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width_, height_, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels_.GetData());
        glGenerateMipmap(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, 0);
    }
    else if (usage_count_ == 0 && gl_binding_ != 0)
    {
        glDeleteTextures(1, &gl_binding_);
        gl_binding_ = 0;
    }
}

void Texture::cleanup()
{
    usage_count_changed();
}