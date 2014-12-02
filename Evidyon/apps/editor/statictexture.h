//------------------------------------------------------------------------------------------------
// File:    statictexture.h
//
// Desc:    
//
// Copyright (c) 2007-2008 Unseen Studios.   All rights reserved.
//------------------------------------------------------------------------------------------------
#ifndef __STATICTEXTURE_H__
#define __STATICTEXTURE_H__


/**
 * Just displays a texture without animated modifications
 */
class StaticTexture : public dc::dcResource<StaticTexture,Texture>
{
    public:

        /**
         * Initializes this class
         */
        StaticTexture();

    public:

        /**
         * Sets up the texture stage indicated on the provided device for rendering
         * @param pd3dDevice The device on which to set up the textures
         * @param stage The stage to configure
         * @return Error result code from D3D, or S_OK
         */
        HRESULT configureTextureStageState(LPDIRECT3DDEVICE9 pd3dDevice, DWORD stage);


    public:

        /**
         * Returns the type name of this class
         */
        static std::string staticTypeName();


    protected:

        /// This class implements the Texture interface
        static dc::dcClass<StaticTexture>::Implements<Texture> implementsTexture;
};


#endif