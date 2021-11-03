#pragma once
#include <OGRE/Main/OgreMaterialManager.h>

class GBufferSchemeHandler : public Ogre::MaterialManager::Listener
{
public:
    Ogre::Technique* handleSchemeNotFound(unsigned short schemeIndex, const Ogre::String& schemeName, Ogre::Material* originalMaterial, unsigned short lodIndex, const Ogre::Renderable* rend) override;

    struct PassProperties 
    {
        PassProperties() : isDeferred(true), normalMap(0), isSkinned(false) {}

        bool isDeferred;
        std::vector<Ogre::TextureUnitState*> regularTextures;
        Ogre::TextureUnitState* normalMap;
        bool isSkinned;
        bool hasDiffuseColour;
        
        //Example of possible extension : vertex colours
        //Ogre::TrackVertexColourType vertexColourType;
    };

    PassProperties inspectPass(Ogre::Pass* pass, unsigned short lodIndex, const Ogre::Renderable* rend);
    bool checkNormalMap(Ogre::TextureUnitState* tus, PassProperties& props);
};
