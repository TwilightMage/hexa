#include "GBufferSchemeHandler.h"

#define OGRE_BUILD_COMPONENT_RTSHADERSYSTEM

#include <OGRE/Main/OgreMaterial.h>
#include <OGRE/Main/OgreTechnique.h>
#include <OGRE/RTShaderSystem/OgreShaderGenerator.h>

const Ogre::String NORMAL_MAP_PATTERN = "normal";

Ogre::Technique* GBufferSchemeHandler::handleSchemeNotFound(unsigned short schemeIndex, const Ogre::String& schemeName, Ogre::Material* originalMaterial, unsigned short lodIndex, const Ogre::Renderable* rend)
{
    Ogre::MaterialManager& matMgr = Ogre::MaterialManager::getSingleton();
    Ogre::String curSchemeName = matMgr.getActiveScheme();
    matMgr.setActiveScheme(Ogre::MaterialManager::DEFAULT_SCHEME_NAME);
    Ogre::Technique* originalTechnique = originalMaterial->getBestTechnique(lodIndex, rend);
    matMgr.setActiveScheme(curSchemeName);
 
#ifndef OGRE_BUILD_COMPONENT_RTSHADERSYSTEM
    Ogre::Technique* gBufferTech = originalMaterial->createTechnique();
    gBufferTech->removeAllPasses();
    gBufferTech->setSchemeName(schemeName);
#endif
 
#ifdef OGRE_BUILD_COMPONENT_RTSHADERSYSTEM
    Ogre::RTShader::ShaderGenerator& rtShaderGen = Ogre::RTShader::ShaderGenerator::getSingleton();
    rtShaderGen.createShaderBasedTechnique(originalTechnique, "NoGBuffer");
    rtShaderGen.createShaderBasedTechnique(originalTechnique, "GBuffer");
#else
    Ogre::Technique* noGBufferTech = originalMaterial->createTechnique();
    noGBufferTech->removeAllPasses();
    noGBufferTech->setSchemeName("NoGBuffer");
#endif
 
    for (unsigned short i=0; i<originalTechnique->getNumPasses(); i++)
    {
        Ogre::Pass* originalPass = originalTechnique->getPass(i);
        PassProperties props = inspectPass(originalPass, lodIndex, rend);
        
        if (!props.isDeferred)
        {
#ifdef OGRE_BUILD_COMPONENT_RTSHADERSYSTEM
            rtShaderGen.validateMaterial("NoGBuffer", *originalMaterial);
#else
            //Just copy the technique so it gets rendered regularly
            Ogre::Pass* clonePass = noGBufferTech->createPass();
            *clonePass = *originalPass;
#endif
            continue;
        }
 
#ifdef OGRE_BUILD_COMPONENT_RTSHADERSYSTEM
        rtShaderGen.validateMaterial("GBuffer", *originalMaterial);
        // Grab the generated technique.
        for(Ogre::Technique* curTech : originalMaterial->getTechniques())
        {
            if (curTech->getSchemeName() == schemeName)
            {
                return curTech;
            }
        }
        break;
#endif
 
        Ogre::Pass* newPass = 0;//gBufferTech->createPass();
        Ogre::MaterialGenerator::Perm perm = getPermutation(props);
 
        const Ogre::MaterialPtr& templateMat = mMaterialGenerator.getMaterial(perm);
        
        //We assume that the GBuffer technique contains only one pass. But its true.
        *newPass = *(templateMat->getTechnique(0)->getPass(0));
        fillPass(newPass, originalPass, props);    
    }
    
    return NULL;// gBufferTech;
}

GBufferSchemeHandler::PassProperties GBufferSchemeHandler::inspectPass(Ogre::Pass* pass, unsigned short lodIndex, const Ogre::Renderable* rend)
{
    PassProperties props;
    
    //TODO : Use renderable to indicate whether this has skinning.
    //Probably use same const cast that renderSingleObject uses.
    if (pass->hasVertexProgram())
    {
        props.isSkinned = pass->getVertexProgram()->isSkeletalAnimationIncluded();
    }
    else 
    {
        props.isSkinned = false;
    }

    for (unsigned short i=0; i<pass->getNumTextureUnitStates(); i++) 
    {
        Ogre::TextureUnitState* tus = pass->getTextureUnitState(i);
        if (!checkNormalMap(tus, props))
        {
            props.regularTextures.push_back(tus);
        }
        if (tus->getEffects().size() > 0)
        {
            props.isDeferred = false;
        }
        
    }

    if (pass->getDiffuse() != Ogre::ColourValue::White)
    {
        props.hasDiffuseColour = true;
    }

    //Check transparency
    if (pass->getDestBlendFactor() != Ogre::SBF_ZERO)
    {
        //TODO : Better ways to do this
        props.isDeferred = false;
    }
    return props;
}

bool GBufferSchemeHandler::checkNormalMap(Ogre::TextureUnitState* tus, PassProperties& props)
{
    bool isNormal = false;
    Ogre::String lowerCaseAlias = tus->getName();
    Ogre::StringUtil::toLowerCase(lowerCaseAlias);
    if (lowerCaseAlias.find(NORMAL_MAP_PATTERN) != Ogre::String::npos)
    {
        isNormal = true;
    }
    else 
    {
        Ogre::String lowerCaseName = tus->getTextureName();
        Ogre::StringUtil::toLowerCase(lowerCaseName);
        if (lowerCaseName.find(NORMAL_MAP_PATTERN) != Ogre::String::npos)
        {
            isNormal = true;
        }
    }

    if (isNormal)
    {
        if (props.normalMap == 0)
        {
            props.normalMap = tus;
        }
        else
        {
            OGRE_EXCEPT(Ogre::Exception::ERR_DUPLICATE_ITEM,
                "Multiple normal map patterns matches",
                "GBufferSchemeHandler::inspectPass");
        }
    }
    return isNormal;
}
