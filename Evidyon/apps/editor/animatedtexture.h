//------------------------------------------------------------------------------------------------
// File:    animatedtexture.h
//
// Desc:    
//
// Copyright (c) 2007-2008 Unseen Studios.   All rights reserved.
//------------------------------------------------------------------------------------------------
#ifndef __ANIMATEDTEXTURE_H__
#define __ANIMATEDTEXTURE_H__

/**
 * This texture iterates through a texture, displaying one frame at a time from it.
 */
class AnimatedTexture : public dc::dcResource<AnimatedTexture,Texture>
{
    public:

        /**
         * Initializes this class
         * @param name The name of this resource
         */
        AnimatedTexture();

    public:

        /**
         * Sets up the texture stage indicated on the provided device for rendering
         * @param pd3dDevice The device on which to set up the textures
         * @param stage The stage to configure
         * @return Error result code from D3D, or S_OK
         */
        HRESULT configureTextureStageState(LPDIRECT3DDEVICE9 pd3dDevice, DWORD stage);

        /**
         * Gets how many frames wide this animated texture is
         */
        int getFramesWideValue() const { return myFramesWide.getValue(); }

        /**
         * Gets how many frames high this animated texture is
         */
        int getFramesHighValue() const { return myFramesHigh.getValue(); }

        /**
         * Gets the speed at which this texture animates
         */
        float getAnimationSpeedValue() const { return myAnimationSpeed.getValue(); }

    public:

        /**
         * Returns the type name of this class
         */
        static std::string staticTypeName();


    protected:

        /// How many frames wide the texture is
        dc::dcByte myFramesWide;

        /// How many frames high the texture is
        dc::dcByte myFramesHigh;

        /// Defines the size of the circling
        dc::dcFloat myAnimationSpeed;


    protected:

        /// This class implements the Texture interface
        static dc::dcClass<AnimatedTexture>::Implements<Texture> implementsTexture;
};


#endif