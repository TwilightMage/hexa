#include "Texture.h"

#include "File.h"
#define STB_IMAGE_STATIC
#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>
#include <glad/glad.h>

#include "Game.h"

std::map<Texture*, uint> Texture::usage_counter_ = std::map<Texture*, uint>();

Texture::Texture(const String& name)
    : Object(name)
    , pixels_(0)
    , width_(0)
    , height_(0)
{
}

Texture::Texture(const String& name, uint width, uint height, List<Color> pixels)
    : Object(name)
    , pixels_(pixels)
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

    assert_error(path.exists(), nullptr, "Texture", "Texture does not exist %s", path.get_absolute_string().c())
    
    int tex_width, tex_height, tex_channels;
    const auto pixels = stbi_load(path.get_absolute_string().c(), &tex_width, &tex_height, &tex_channels, STBI_rgb_alpha);
    if (pixels)
    {
        if (tex_width * tex_height > 0)
        {
            const uint size = tex_width * tex_height;
            auto result = MakeShared<Texture>(path.get_absolute_string());
            result->pixels_ = List(new Color[size], size);
            memcpy(result->pixels_.get_data(), pixels, sizeof(Color) * size);
            stbi_image_free(pixels);
            result->width_ = tex_width;
            result->height_ = tex_height;

            Game::instance_->textures_[path.get_absolute_string()] = result;
            verbose("Texture", "Loaded texture %ix%i %s", tex_width, tex_height, path.get_absolute_string().c());
            
            return result;
        }
        else
        {
            print_error("Texture", "Texture size is invalid: %ix%i %s", tex_width, tex_height, path.get_absolute_string().c());
        }
    }
    else
    {
        print_error("Texture", "Unknown error on loading texture %s", path.get_absolute_string().c());
    }
    stbi_image_free(pixels);
    return nullptr;
}

uint Texture::get_gl_texture_id()
{
    return gl_texture_binding_;
}

uint64 Texture::get_handle_arb() const
{
    return handle_arb_;
}

uint Texture::get_width() const
{
    return width_;
}

uint Texture::get_height() const
{
    return height_;
}

const std::map<Texture*, uint>& Texture::get_usage_counter()
{
    return usage_counter_;
}

void Texture::usage_count_increase()
{
    usage_counter_[this]++;
    usage_count_changed();
}

void Texture::usage_count_decrease()
{
    auto& usage_count = usage_counter_[this];
    usage_count--;
    usage_count_changed();
    if (usage_count == 0)
    {
        usage_counter_.erase(this);
    }
}

void Texture::usage_count_changed()
{
    const auto usage_count = usage_counter_[this];
    if (usage_count > 0 && gl_texture_binding_ == 0)
    {
        glGenTextures(1, &gl_texture_binding_);
        glBindTexture(GL_TEXTURE_2D, gl_texture_binding_);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width_, height_, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels_.get_data());
        glGenerateMipmap(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, 0);

        handle_arb_ = glGetTextureHandleARB(gl_texture_binding_);
        glMakeTextureHandleResidentARB(handle_arb_);
    }
    else if (usage_count == 0 && gl_texture_binding_ != 0)
    {
        glMakeTextureHandleNonResidentARB(handle_arb_);
        handle_arb_ = 0;
        glDeleteTextures(1, &gl_texture_binding_);
        gl_texture_binding_ = 0;
    }
}

void Texture::cleanup()
{
    auto& usage_count = usage_counter_[this];
    usage_count = 0;
    usage_count_changed();
    if (usage_count == 0)
    {
        usage_counter_.erase(this);
    }
}
