#include "TextureAtlas.h"

#define STB_IMAGE_STATIC
#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <glad/glad.h>
#include <stb/stb_image_write.h>

#include "File.h"
#include "Game.h"
#include "ImageEditor.h"

int TextureAtlas::max_size_ = 0;

TextureAtlas::TextureAtlas(const String& name)
    : Object(name)
    , cell_width_(1)
    , cell_height_(1)
{
}

TextureAtlas::TextureAtlas(const String& name, int cell_width, int cell_height)
    : Object(name)
    , cell_width_(cell_width)
    , cell_height_(cell_height)
{
}

Vector2 TextureAtlas::Entry::get_scale() const
{
    return Vector2(static_cast<float>(rect.w) / owner->size_, static_cast<float>(rect.h) / owner->size_);
}

Vector2 TextureAtlas::Entry::get_offset() const
{
    return Vector2(static_cast<float>(rect.x) / owner->size_, static_cast<float>(rect.y) / owner->size_);
}

uint TextureAtlas::put(const Path& path)
{
    if (!Game::is_loading_stage())
    {
        print_warning("texture atlas", "attempt to register texture %s in atlas %s outside of loading stage", path.get_absolute_string().c(), get_name().c());
        return -1;
    }
    
    if (path.exists())
    {
        int tex_width, tex_height, tex_channels;
        const auto pixels = stbi_load(path.get_absolute_string().c(), &tex_width, &tex_height, &tex_channels, STBI_rgb_alpha);
        if (pixels)
        {
            auto image_rect = Rect(0, 0, tex_width, tex_height);

            /*if (entries_.Length() > 0)
            {
                auto& last = entries_[entries_.Length() - 1];
                image_rect.x = last.rect.x + cell_width_;
                image_rect.y = last.rect.y;
            }*/
            
            auto desired_size = size_;
            auto found = false;
            while (true)
            {
                if (desired_size > tex_width && desired_size > tex_height)
                {
                    for (; image_rect.y < desired_size - image_rect.h; image_rect.y += cell_height_)
                    {
                        for (; image_rect.x < desired_size - image_rect.w; image_rect.x += cell_width_)
                        {
                            if (entries_.length() > 0)
                            {
                                auto any_intersection = false;
                                for (auto& entry : entries_)
                                {
                                    if (image_rect.intersects(entry.rect))
                                    {
                                        any_intersection = true;
                                        break;
                                    }
                                }
                        
                                if (!any_intersection)
                                {
                                    found = true;
                                    break;
                                }
                            }
                            else
                            {
                                found = true;
                                break;
                            }
                        }
                        if (found) break;
                        image_rect.x = 0;
                    }
                    if (found) break;
                }
                if (desired_size * 2 > max_size_) throw std::runtime_error("Unable to put image " + path.get_absolute_string().std() + " into atlas due to size excess");
                desired_size *= 2;
            }

            if (found)
            {
                if (desired_size > size_)
                {
                    auto new_pixels = List<Color>(desired_size * desired_size);
                    ImageEditor::copy_rect(pixels_.get_data(), 0, 0, size_, new_pixels.get_data(), 0, 0, desired_size, size_, size_);
                    pixels_ = new_pixels;
                    size_ = desired_size;

                    cached_uv_mods_.Clear();
                    for (const auto& entry : entries_)
                    {
                        cached_uv_mods_.Add({entry.get_scale(), entry.get_offset()});
                    }
                }

                ImageEditor::copy_rect(reinterpret_cast<Color*>(pixels), 0, 0, tex_width, pixels_.get_data(), image_rect.x, image_rect.y, size_, image_rect.w, image_rect.h);
                stbi_image_free(pixels);

                const Entry new_entry = {image_rect, this};
                entries_.Add(new_entry);
                cached_uv_mods_.Add({new_entry.get_scale(), new_entry.get_offset()});
                return entries_.length() - 1;
            }            
        }
        stbi_image_free(pixels);
    }

    return -1;
}

uint TextureAtlas::claim_rect(const Rect& rect)
{
    for (const auto& entry : entries_)
    {
        if (rect.intersects(entry.rect))
        {
            return -1;
        }
    }

    entries_.Add({rect, this});
    return entries_.length() - 1;
}

uint TextureAtlas::get_num_entries() const
{
    return entries_.length();
}

List<TextureAtlas::UVMod> TextureAtlas::get_cached_mods() const
{
    return cached_uv_mods_;
}

const TextureAtlas::Entry* TextureAtlas::get_entry(uint index) const
{
    return &entries_[index];
}

void TextureAtlas::dump(const Path& path)
{
    stbi_write_bmp(path.get_absolute_string().c(), size_, size_, 4, pixels_.get_data());
}

Shared<Texture> TextureAtlas::to_texture() const
{
    return MakeShared<Texture>(get_name(), size_, size_, pixels_);
}

void TextureAtlas::bind(uint storage_slot) const
{
    if (Game::is_render_stage())
    {
        glBindTexture(GL_TEXTURE_2D, gl_texture_binding_);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, storage_slot, gl_mods_storage_binding_);
    }
}

uint TextureAtlas::get_gl_texture_id()
{
    return gl_texture_binding_;
}

Shared<TextureAtlas> TextureAtlas::load_png(const Path& path)
{
    int tex_width, tex_height, tex_channels;
    const auto pixels = stbi_load(path.get_absolute_string().c(), &tex_width, &tex_height, &tex_channels, STBI_rgb_alpha);
    if (pixels)
    {
        if (tex_width * tex_height > 0)
        {
            if (tex_width == tex_height && (tex_width & (tex_width - 1)) == 0)
            {
                const uint size = tex_width * tex_height;
                auto result = MakeShared<TextureAtlas>(path.get_absolute_string());
                result->pixels_ = List(new Color[size], size);
                memcpy(result->pixels_.get_data(), pixels, sizeof(Color) * size);
                stbi_image_free(pixels);
                result->size_ = tex_width;

                verbose("Texture Atlas", "Loaded texture atlas %i %s", tex_width, path.get_absolute_string().c());
            
                return result;
            }
            else
            {
                print_error("Texture Atlas", "Texture atlas size must be power of 2 %s", path.get_absolute_string().c());
            }
        }
        else
        {
            print_error("Texture Atlas", "Texture atlas size is invalid: %ix%i %s", tex_width, tex_height, path.get_absolute_string().c());
        }
    }
    else
    {
        print_error("Texture Atlas", "Unknown error on loading texture %s", path.get_absolute_string().c());
    }
    stbi_image_free(pixels);
    return nullptr;
}

void TextureAtlas::generate_buffers()
{
    if (gl_texture_binding_ || gl_mods_storage_binding_)
    {
        print_warning("Texture Atlas", "Attempt to generate buffers while texture buffer is %s and storage buffer is %s", gl_texture_binding_ ? "active" : "inactive", gl_mods_storage_binding_ ? "active" : "inactive");
    }
    
    cleanup();
    
    glGenTextures(1, &gl_texture_binding_);
    glBindTexture(GL_TEXTURE_2D, gl_texture_binding_);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, size_, size_, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels_.get_data());
    glGenerateMipmap(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 0);

    glGenBuffers(1, &gl_mods_storage_binding_);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, gl_mods_storage_binding_);
    glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(UVMod) * cached_uv_mods_.length(), cached_uv_mods_.get_data(), GL_STATIC_DRAW);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

void TextureAtlas::cleanup()
{
    if (gl_texture_binding_)
    {
        glDeleteTextures(1, &gl_texture_binding_);
        gl_texture_binding_ = 0;
    }

    if (gl_mods_storage_binding_)
    {
        glDeleteBuffers(1, &gl_mods_storage_binding_);
        gl_mods_storage_binding_ = 0;
    }
}
