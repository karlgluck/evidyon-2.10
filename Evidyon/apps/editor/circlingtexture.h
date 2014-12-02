//------------------------------------------------------------------------------------------------
// File:    texture.h
//
// Desc:    
//
// Copyright (c) 2007-2008 Unseen Studios.   All rights reserved.
//------------------------------------------------------------------------------------------------
#ifndef __CIRCLINGTEXTURE_H__
#define __CIRCLINGTEXTURE_H__



class CirclingTexture : public dc::dcResource<CirclingTexture,Texture>
{
    public:

        /**
         * Initializes this class
         * @param name The name of this resource
         */
        CirclingTexture();

    public:

        /**
         * Sets up the texture stage indicated on the provided device for rendering
         * @param pd3dDevice The device on which to set up the textures
         * @param stage The stage to configure
         * @return Error result code from D3D, or S_OK
         */
        HRESULT configureTextureStageState(LPDIRECT3DDEVICE9 pd3dDevice, DWORD stage);

        /**
         * Gets the value of the speed at which this texture circles
         */
        float getCirclingSpeedValue() const { return myCirclingSpeed.getValue(); }

        /**
         * Gets the value of the magnitude at which this texture circles
         */
        float getCirclingMangitudeValue() const { return myCirclingMagnitude.getValue(); }


    public:

        /**
         * Returns the type name of this class
         */
        static std::string staticTypeName();


    protected:

        /// How fast this texture moves.  If this is zero, all other movement parameters are
        /// ignored.  If this is negative, the parameter defines
        dc::dcFloat myCirclingSpeed;

        /// Defines the size of the circling
        dc::dcFloat myCirclingMagnitude;


    protected:

        /// This class implements the Texture interface
        static dc::dcClass<CirclingTexture>::Implements<Texture> implementsTexture;
};


#endif