#pragma once

#include "Array2D.h"
#include "Color.h"
#include "Map.h"
#include "ModuleAssetID.h"
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
    explicit Module(const Name& module_name);
    
    virtual void on_add_resource_directories(Set<String>& local, Set<String>& global);

    FORCEINLINE const Name& get_module_name() const { return module_name; }
    FORCEINLINE const ModuleAssetID get_asset_id(const Name& asset_name) const { return ModuleAssetID(get_module_name(), asset_name); }

    virtual Path get_module_path(const String& sub_path = "") const = 0;

private:
    void add_resource_directories();
    
    void register_resource_directories();
    void unregister_resource_directories();

    static void reset_global_resources_directories();

    Name module_name;

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
};
