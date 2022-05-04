/*
-----------------------------------------------------------------------------
This source file is part of OGRE
(Object-oriented Graphics Rendering Engine)
For the latest info, see http://www.ogre3d.org/

Copyright (c) 2000-2014 Torus Knot Software Ltd
Also see acknowledgements in Readme.html

You may use this sample code for anything you like, it is not covered by the
same license as the rest of the engine.
-----------------------------------------------------------------------------
*/

#ifndef H_WJ_DLight
#define H_WJ_DLight

#include <OGRE/OgreSimpleRenderable.h>

#include "MaterialGenerator.h"

/** Deferred light geometry. Each instance matches a normal light.
    Should not be created by the user.
    XXX support other types of light other than point lights.
 */
class DLight: public Ogre::SimpleRenderable
{
public:
    DLight(MaterialGenerator *gen, Ogre::Light* parentLight);
    ~DLight() override;

    /** Update the information from the light that matches this one 
     */
    void updateFromParent();

    /** Update the information that is related to the camera
     */
    void updateFromCamera(Ogre::Camera* camera);

    /** Does this light cast shadows?
    */
    virtual bool getCastChadows() const;

    /** @copydoc MovableObject::getBoundingRadius */
    Ogre::Real getBoundingRadius(void) const override;
    /** @copydoc Renderable::getSquaredViewDepth */
    Ogre::Real getSquaredViewDepth(const Ogre::Camera*) const override;
    /** @copydoc Renderable::getMaterial */
    const Ogre::MaterialPtr& getMaterial(void) const override;
    /** @copydoc Renderable::getBoundingRadius */
    void getWorldTransforms(Ogre::Matrix4* xform) const override;
protected:

    /** Check if the camera is inside a light
    */
    bool isCameraInsideLight(Ogre::Camera* camera);

    /** Create geometry for this light.
    */
    void rebuildGeometry(float radius);

    /** Create a sphere geometry.
    */
    void createSphere(float radius, int nRings, int nSegments);

    /** Create a rectangle.
    */
    void createRectangle2D();
    
    /** Create a cone.
    */
    void createCone(float radius, float height, int nVerticesInBase);

    /** Set constant, linear, quadratic Attenuation terms 
     */
    void setAttenuation(float c, float b, float a);

    /** Set the specular colour
     */
    void setSpecularColour(const Ogre::ColourValue &col);

    /// The light that this DLight renders
    Ogre::Light* mParentLight;
    /// Mode to ignore world orientation/position
    bool bIgnoreWorld;
    /// Bounding sphere radius
    float mRadius;
    /// Deferred shading system this minilight is part of
    MaterialGenerator *mGenerator;
    /// Material permutation
    Ogre::uint32 mPermutation;
};

#endif
