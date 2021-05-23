#include "Texture.h"

#include "File.h"
#define STB_IMAGE_STATIC
#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>
#include <glad/glad.h>

#include "Game.h"

Texture::Editor::Editor(const Editor& rhs)
    : target_(rhs.target_)
{
    target_->edit_count_increase();
}

Texture::Editor& Texture::Editor::operator=(const Editor& rhs)
{
    target_ = rhs.target_;
    target_->edit_count_increase();
    return *this;
}

Texture::Editor::~Editor()
{
    target_->edit_count_--;
}

void Texture::Editor::set_pixel(uint x, uint y, const Color& color) const
{
    target_->pixels_[y * target_->width_ * x] = color;
}

void Texture::Editor::process_pixels(Color(* processor)(uint x, uint y, const Color&)) const
{
    for (uint i = 0; i < target_->pixels_.length(); i++)
    {
        target_->pixels_[i] = processor(i % target_->width_, i / target_->width_, target_->pixels_[i]);
    }
}

Texture::Editor::Editor(const Shared<Texture>& target)
    : target_(target)
{
    assert(target_ != nullptr);
    target_->edit_count_increase();
}

Texture::Texture(const String& name)
    : Object(name)
    , pixels_(0)
    , edit_count_(0)
    , delayed_activation_(false)
{
}

Texture::Texture(const String& name, uint width, uint height, List<Color> pixels)
    : Object(name)
    , pixels_(pixels)
    , width_(width)
    , height_(height)
    , edit_count_(0)
    , delayed_activation_(false)
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

Vector2 Texture::get_size() const
{
    return Vector2(static_cast<float>(width_), static_cast<float>(height_));
}

Color Texture::get_pixel(uint x, uint y) const
{
    return pixels_[y * width_ + x];
}

Shared<Texture::Editor> Texture::edit()
{
    if (usage_count() == 0) return MakeSharedInternal(Editor, shared_from_this());
    return nullptr;
}

void Texture::load()
{
    if (edit_count_ == 0)
    {
        load_internal();
    }
    else
    {
        delayed_activation_ = true;
    }
}

void Texture::unload()
{
    glMakeTextureHandleNonResidentARB(handle_arb_);
    handle_arb_ = 0;
    glDeleteTextures(1, &gl_texture_binding_);
    gl_texture_binding_ = 0;
}

void Texture::load_internal()
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

    delayed_activation_ = false;
}

void Texture::edit_count_increase()
{
    edit_count_++;
}

void Texture::edit_count_decrease()
{
    edit_count_--;
    if (edit_count_ == 0 && delayed_activation_)
    {
        load_internal();
    }
}
