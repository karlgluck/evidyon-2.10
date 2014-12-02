//------------------------------------------------------------------------------------------------
// File:    slidingtexture.h
//
// Desc:    
//
// Copyright (c) 2007-2008 Unseen Studios.   All rights reserved.
//------------------------------------------------------------------------------------------------
#ifndef __SLIDINGTEXTURE_H__
#define __SLIDINGTEXTURE_H__


class SlidingTexture : public dc::dcResource<SlidingTexture,Texture>
{
    public:

        /**
         * Initializes this class
         */
        SlidingTexture();


    public:

        /**
         * Sets up the texture stage indicated on the provided device for rendering
         * @param pd3dDevice The device on which to set up the textures
         * @param stage The stage to configure
         * @return Error result code from D3D, or S_OK
         */
        HRESULT configureTextureStageState(LPDIRECT3DDEVICE9 pd3dDevice, DWORD stage);

        /**
         * Gets the value of the speed at which this texture slides
         */
        float getMovementSpeedValue() const { return myMovementSpeed.getValue(); }

        /**
         * Gets the value of the angle (in radians) at which this texture slides
         */
        float getMovementAngleValue() const { return myMovementAngle.getValue(); }

    public:

        /**
         * Returns the type name of this class
         */
        static std::string staticTypeName();


    protected:

        /// How fast this texture moves.
        dc::dcFloat myMovementSpeed;

        /// What angle the texture moves in.  This will just perform a straight
        /// transform of the texture coordinates, so wrapping has to be enabled.
        dc::dcFloat myMovementAngle;


    protected:

        /// This class implements the Texture interface
        static dc::dcClass<SlidingTexture>::Implements<Texture> implementsTexture;
};


#endif