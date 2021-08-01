#include "Cubemap.h"

#include "Array2D.h"
#include "stb.h"
#include "Utils.h"

Shared<Cubemap> Cubemap::load_png(const Path& p_x, const Path& n_x, const Path& p_y, const Path& n_y, const Path& p_z, const Path& n_z)
{
    List paths = {p_x, n_x, p_y, n_y, p_z, n_z};

    List<Array2D<Color>> faces = List<Array2D<Color>>(6);

    uint width, height;
    
    for (uint i = 0; i < 6; i++)
    {
        uint w, h;
        auto pixels = stb::load(paths[i], w, h);

        if (i == 0)
        {
            width = w;
            height = h;
        }
        else
        {
            if (width != w || height != h)
            {
                print_error("Cubemap", "All textures have to be same size. Texture %s differs", paths[i].get_absolute_string().c());
                return nullptr;
            }
        }
        
        faces[i] = Array2D<Color>(w, h, pixels);
        
        if (w == 0 || h == 0)
        {
            print_error("Cubemap", "Failed to load texture %s", paths[i].get_absolute_string().c());
            return nullptr;
        }
    }

    Shared<Cubemap> result = MakeShared<Cubemap>();
    
    /*glGenTextures(1, &result->gl_id);
    glBindTexture(GL_TEXTURE_CUBE_MAP, result->gl_id);
    
    for (uint i = 0; i < 6; i++)
    {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA, faces[i].get_size_x(), faces[i].get_size_y(), 0, GL_RGBA, GL_UNSIGNED_BYTE, faces[i].to_list().get_data());
    }

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);*/

    return result;
}
