#pragma once

#include "Pointers.h"

class Texture;
class TileInfo;
class MeshComponent;
class Module;

namespace Ogre {
    class Pass;
    class Technique;
    class Material;
}

class EXPORT Material
{
    friend Module;
    friend MeshComponent;
public:
    class Technique
    {
    public:
        class Pass
        {
        public:
            
        
        private:
            Ogre::Pass* ogre_pass_ = nullptr;
        };

        Shared<Pass> get_pass(uint index) const;
        Shared<Pass> get_pass(const String& name) const;
    
    private:
        Ogre::Technique* ogre_technique_ = nullptr;
    };
    
    Shared<Material> clone(const String& new_name = "") const;

    void set_texture(const Shared<Texture>& texture, uint index);
    void set_texture(const String& texture, uint index);
    
private:
    Shared<Ogre::Material> ogre_material_;
};
