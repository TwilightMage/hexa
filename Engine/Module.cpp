#include "Module.h"

#include <OGRE/Main/OgreMaterialManager.h>
#include <OGRE/Main/OgreResourceGroupManager.h>
#include <OGRE/Main/OgreTextureManager.h>

#include "Logger.h"
#include "Material.h"

Module::Module(const Path& root, const String& module_name)
    : root(root)
    , module_name(module_name)
{
}

void Module::on_add_resource_directories(Set<String>& local, Set<String>& global)
{
}

Shared<Material> Module::get_material(const String& name) const
{
    if (auto ogre_material = Ogre::MaterialManager::getSingleton().getByName(name.c(), module_name.c()))
    {
        auto result = MakeShared<Material>();
        result->ogre_material_ = ogre_material;
        return result;
    }

    return nullptr;
}

void Module::create_texture(const Array2D<Color>& pixels, const String& name)
{
    Ogre::Image ogre_image(Ogre::PF_R5G6B5, pixels.get_size_x(), pixels.get_size_y());

    for (uint x = 0; x < pixels.get_size_x(); x++)
    {
        for (uint y = 0; y < pixels.get_size_y(); y++)
        {
            ogre_image.setColourAt(Ogre::ColourValue((byte*)&pixels.at(x, y)), x, y, 0);
        }
    }

    Ogre::TextureManager::getSingleton().loadImage(name.c(), get_module_name().c(), ogre_image);
}

void Module::add_resource_directories()
{
    Set<String> local;
    Set<String> global;

    on_add_resource_directories(local, global);

    local_directories += local;
    global_directories += global;
}

void Module::register_resource_directories()
{
    const Set<String> directories = local_directories + global_directories;
    for (const auto& directory : directories)
    {
        const auto path = Path(root.get_absolute_string() + "/resources/" + directory);
        if (path.exists())
        {
            Ogre::ResourceGroupManager::getSingletonPtr()->addResourceLocation(path.get_absolute_string().c(), "FileSystem", module_name.c());
        }
    }
}

void Module::unregister_resource_directories()
{
    const auto locations = Ogre::ResourceGroupManager::getSingletonPtr()->listResourceLocations(module_name.c());
    for (uint i = 0; i < locations->size(); i++)
    {
        Ogre::ResourceGroupManager::getSingletonPtr()->removeResourceLocation(locations->at(i), module_name.c());
    }
}

void Module::reset_global_resources_directories()
{
    global_directories = global_directories_default;
}
