﻿#include "TextureAtlas.h"

#define STB_IMAGE_STATIC
#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <glad/glad.h>
#include <stb/stb_image_write.h>

#include "ImageEditor.h"

int TextureAtlas::max_size_ = 0;

TextureAtlas::TextureAtlas()
    : cell_width_(1)
    , cell_height_(1)
{
}

TextureAtlas::TextureAtlas(int cell_width, int cell_height)
    : cell_width_(cell_width)
    , cell_height_(cell_height)
{
}

Vector2 TextureAtlas::entry::get_scale() const
{
    return Vector2(static_cast<float>(rect.w) / owner->size_, static_cast<float>(rect.h) / owner->size_);
}

Vector2 TextureAtlas::entry::get_offset() const
{
    return Vector2(static_cast<float>(rect.x) / owner->size_, static_cast<float>(rect.y) / owner->size_);
}

uint TextureAtlas::put(const Path& path)
{
    if (path.exists())
    {
        int tex_width, tex_height, tex_channels;
        const auto pixels = stbi_load(path.get_absolute_string().c(), &tex_width, &tex_height, &tex_channels, STBI_rgb_alpha);
        if (pixels)
        {
            const uint tex_size = tex_width * tex_height * 4;

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
                            if (entries_.Length() > 0)
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
                    ImageEditor::copy_rect(pixels_.GetData(), 0, 0, size_, new_pixels.GetData(), 0, 0, desired_size, size_, size_);
                    pixels_ = new_pixels;
                    size_ = desired_size;

                    cached_uv_mods.Clear();
                    for (const auto& entry : entries_)
                    {
                        cached_uv_mods.Add({entry.get_scale(), entry.get_offset()});
                    }
                }

                ImageEditor::copy_rect(reinterpret_cast<Color*>(pixels), 0, 0, tex_width, pixels_.GetData(), image_rect.x, image_rect.y, size_, image_rect.w, image_rect.h);
                stbi_image_free(pixels);

                const entry new_entry = {image_rect, this};
                entries_.Add(new_entry);
                cached_uv_mods.Add({new_entry.get_scale(), new_entry.get_offset()});
                return entries_.Length() - 1;
            }            
        }
        stbi_image_free(pixels);
    }

    return -1;
}

uint TextureAtlas::get_num_entries() const
{
    return entries_.Length();
}

List<TextureAtlas::uv_mod> TextureAtlas::get_cached_mods() const
{
    return cached_uv_mods;
}

const TextureAtlas::entry* TextureAtlas::get_entry(uint index) const
{
    return &entries_[index];
}

void TextureAtlas::dump(const Path& path)
{
    stbi_write_bmp(path.get_absolute_string().c(), size_, size_, 4, pixels_.GetData());
}

Shared<Texture> TextureAtlas::to_texture() const
{
    return MakeShared<Texture>(size_, size_, pixels_);
}

uint TextureAtlas::get_gl_id()
{
    return gl_binding_;
}

void TextureAtlas::usage_count_changed()
{
    if (usage_count_ > 0 && gl_binding_ == 0)
    {
        glGenTextures(1, &gl_binding_);
        glBindTexture(GL_TEXTURE_2D, gl_binding_);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, size_, size_, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels_.GetData());
        glGenerateMipmap(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, 0);
    }
    else if (usage_count_ == 0 && gl_binding_ != 0)
    {
        glDeleteTextures(1, &gl_binding_);
        gl_binding_ = 0;
    }
}

void TextureAtlas::cleanup()
{
    usage_count_changed();
}