#pragma once

#include "Array2D.h"
#include "Color.h"
#include "Map.h"
#include "Name.h"
#include "Path.h"
#include "Pointers.h"
#include "Set.h"

class Texture;
class Material;
class Game;

class EXPORT Module
{
    friend Game;

public:
    Module(const Path& root, const String& module_name);
    
    virtual void on_add_resource_directories(Set<String>& local, Set<String>& global);

    FORCEINLINE const String& get_module_name() const { return module_name; }

    Shared<Material> load_material(const Name& name);

    Shared<Texture> create_texture(const Array2D<Color>& pixels, const String& name);
    Shared<Texture> get_texture(const String& name) const;

private:
    void add_resource_directories();
    
    void register_resource_directories();
    void unregister_resource_directories();

    static void reset_global_resources_directories();

    Path root;
    String module_name;

    Set<String> local_directories = Set<String>();
    inline static Set<String> global_directories = Set<String>();
    inline static Set<String> global_directories_default = {
        "textures",
        "shaders",
        "materials",
        "meshes",
        "audio",
        "fonts"
    };

    Map<Name, Shared<Material>> loaded_materials_;
};
