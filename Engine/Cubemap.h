#pragma once

#include "IUsageCountable.h"
#include "Pointers.h"

class Path;

class EXPORT Cubemap
{
public:

    static Shared<Cubemap> load_png(const Path& p_x, const Path& n_x, const Path& p_y, const Path& n_y, const Path& p_z, const Path& n_z);
    FORCEINLINE static Shared<Cubemap> load_png(const Path& hor, const Path& p_z, const Path& n_z) { return load_png(hor, hor, hor, hor, p_z, n_z); }
    FORCEINLINE static Shared<Cubemap> load_png(const Path& face) { return load_png(face, face, face, face, face, face); }

    FORCEINLINE uint get_gl_id() const { return gl_id; }
    
private:
    uint gl_id = -1;
};
