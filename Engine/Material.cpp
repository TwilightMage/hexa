#include "Material.h"

#include <OGRE/Main/OgreMaterial.h>
#include <OGRE/Main/OgreTechnique.h>

Shared<Material> Material::clone(const String& new_name) const
{
    auto result = MakeShared<Material>();
    result->ogre_material_ = ogre_material_->clone(new_name.is_empty() ? ogre_material_->getName() + "_Clone" : new_name.c());
    return result;
}

/*void Material::set_texture(const Shared<Texture>& texture, uint index)
{
    ogre_material_->getTechnique(0)->getPass(0)->getTextureUnitState(index)->setTexture(texture->tex);
}*/

void Material::set_texture(const String& texture, uint index)
{
    ogre_material_->getTechnique(0)->getPass(0)->getTextureUnitState(index)->setTextureName(texture.c());
}
