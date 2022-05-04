#include "Texture.h"

#include <OGRE/OgreTextureManager.h>

#include "Assert.h"
#include "File.h"
#include "Game.h"
#include "stb.h"

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
    target_->pixels_.at(x, y) = color;
}

void Texture::Editor::process_pixels(Color(* processor)(uint x, uint y, const Color&)) const
{
    for (uint x = 0; x < target_->pixels_.get_size_x(); x++)
    {
        for (uint y = 0; y < target_->pixels_.get_size_y(); y++)
        {
            target_->pixels_.at(x, y) = processor(x, y, target_->pixels_.at(x, y));
        }
    }
}

Texture::Editor::Editor(const Shared<Texture>& target)
    : target_(target)
{
    assert(target_ != nullptr);
    target_->edit_count_increase();
}

Texture::Texture()
    : edit_count_(0)
    , delayed_activation_(false)
{
}

Texture::Texture(const Array2D<Color>& pixels)
    : pixels_(pixels)
    , edit_count_(0)
    , delayed_activation_(false)
{
}

Texture::Texture(uint width, uint height, const List<Color>& pixels)
    : Texture(Array2D(width, height, pixels))
{
}

Shared<Texture> Texture::load_png(const Path& path)
{
    assert(false);
    return nullptr;
    
    /*if (const auto found = Game::instance_->textures_.find(path.get_absolute_string()))
    {
        return *found;
    }

    if (!Check(path.exists(), "Texture Loader", "Texture does not exist %s", path.get_absolute_string().c())) return nullptr;
    
    uint tex_width, tex_height, tex_channels;
    const auto pixels = stb::load(path, tex_width, tex_height);
    if (pixels.length() > 0)
    {
        if (tex_width * tex_height > 0)
        {
            const uint size = tex_width * tex_height;
            auto result = MakeShared<Texture>(path.get_absolute_string());
            result->pixels_ = Array2D(tex_width, tex_height, pixels);

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
    return nullptr;*/
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
    return pixels_.get_size_x();
}

uint Texture::get_height() const
{
    return pixels_.get_size_y();
}

Vector2 Texture::get_size() const
{
    return Vector2(static_cast<float>(pixels_.get_size_x()), static_cast<float>(pixels_.get_size_y()));
}

Color Texture::get_pixel(uint x, uint y) const
{
    return pixels_.at(x, y);
}

void Texture::save_to_file(const Path& path)
{
    stb::write_bmp(path, pixels_.get_size_x(), pixels_.get_size_y(), pixels_.to_list());
}

void Texture::put_pixels(const Array2D<Color>& pixels)
{
    put_pixels(pixels.get_size_x(), pixels.get_size_y(), pixels.to_list());
}

void Texture::put_pixels(uint width, uint height, const List<Color>& pixels)
{
    if (edit_count_ == 0 && pixels_.get_size_x() == width && pixels_.get_size_y() == height)
    {
        pixels_ = Array2D(width, height, pixels);

        if (usage_count() > 0)
        {
            unload();
            load_internal();
        }
    }
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
    /*glMakeTextureHandleNonResidentARB(handle_arb_);
    handle_arb_ = 0;
    glDeleteTextures(1, &gl_texture_binding_);
    gl_texture_binding_ = 0;*/
}

void Texture::load_internal()
{
    /*glGenTextures(1, &gl_texture_binding_);
    glBindTexture(GL_TEXTURE_2D, gl_texture_binding_);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, pixels_.get_size_x(), pixels_.get_size_y(), 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels_.to_list().get_data());
    glGenerateMipmap(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 0);

    handle_arb_ = glGetTextureHandleARB(gl_texture_binding_);
    glMakeTextureHandleResidentARB(handle_arb_);*/

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
