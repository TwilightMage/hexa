#include "Module.h"

#include <OGRE/Main/OgreMaterialManager.h>
#include <OGRE/Main/OgreResourceGroupManager.h>

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
    auto result = MakeShared<Material>();
    result->ogre_material_ = Ogre::MaterialManager::getSingleton().getByName(name.c(), module_name.c());
    return result;
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
