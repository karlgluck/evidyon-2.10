//------------------------------------------------------------------------------------------------
// File:  isometriccamera.h
//
// Desc:  Sets up the transform states for view rendering and manages view-based information
//
// Copyright (c) 2007-2008 Unseen Studios.   All rights reserved.
//------------------------------------------------------------------------------------------------
#ifndef __ISOMETRICCAMERA_H__
#define __ISOMETRICCAMERA_H__

#include <d3dx9.h>

/**
 * Sets up the transform states for view rendering and manages view-based information
 *   @author Karl Gluck
 */
class IsometricCamera
{
  public:

    /**
     * Resets this class
     */
    IsometricCamera();

    /**
     * Sets the location of the camera on the map
     */
    void setPosition(float x, float z);

    /**
     * Changes whether or not this camera looks down on the world from above
     * or in 3/4 view.
     *   @param enable Whether or not to view from above
     */
    void enableTopdownView(bool enable);
    bool topdownViewEnabled() const;

    void rebuildXenimusCamera(DWORD screen_width, DWORD screen_height);

    /**
     * Reconstructs the internal matrices and values based on the current device
     * @param screenWidth How wide the screen is
     * @param screenHeight How high the screen is
     * @param zoom Controls the distance to the point of focus.  Most of the time,
     *       this should be 1.0f because that's the distance at which the game
     *       will be played.
     */
    void rebuild(DWORD screenWidth, DWORD screenHeight, float zoom = 1.0f, float params[5] = 0);

    /**
     * Reconstructs the internal matrices and values based on the current device
     * @param screenWidth How wide the screen is
     * @param screenHeight How high the screen is
     * @param zoom Controls the distance to the point of focus.  Most of the time,
     *       this should be 1.0f because that's the distance at which the game
     *       will be played.
     */
    void rebuildAs3D(DWORD screenWidth, DWORD screenHeight, float angle, float zoom = 1.0f);

    /**
     * Sets up the destination device for rendering using this camera
     *   @param d3dDevice The Direct3D device to initialize
     *   @return Result code
     */
    bool initialize3DRendering(LPDIRECT3DDEVICE9 d3dDevice);

    /**
     * Obtains the world matrix
     */
    //const D3DXMATRIX* getViewMatrix() const;

    /**
     * Obtains the projection matrix
     */
    //const D3DXMATRIX* getProjMatrix() const;

    /**
     * Sets the designated world matrix on the device.  This method must be used to set the world
     * matrix because the camera does not move in the world; the world moves around the camera.
     * @param d3dDevice The destination Direct3D device
     * @param worldMatrix The desired world matrix
     * @param index The index of the world matrix to set; zero is equivalent to setting D3DTS_WORLD
     * @return Whether or not the operation succeeded
     */
    bool setWorldMatrix(LPDIRECT3DDEVICE9 d3dDevice, const D3DXMATRIX* worldMatrix, DWORD index = 0);

    /**
     * Ray-casts a screen coordinate into the corresponding location on the map
     * @param screenX The X-coordinate on the screen
     * @param screenY The Y-coordinate on the screen
     * @param worldX Output x-coordinate on the map
     * @param worldY Output z-coordinate on the map
     */
    void getWorldLocation(int screenX, int screenY, float* worldX, float* worldZ);

    /**
     * Projects a point in the world to its corresponding location on the screen
     */
    void getScreenLocation(float worldX, float worldY, float worldZ, float* screenX, float* screenY);


  protected:

    /**
     * Resets the class
     */
    void zero();


  protected:

    /// How big the screen is
    DWORD myScreenWidth, myScreenHeight;

    /// Whether or not to look in isometric mode on the world
    bool myTiltWorldFlag;

    /// The constructed view matrix
    D3DXMATRIXA16 myViewMatrix;

    /// The constructed perspective matrix
    D3DXMATRIXA16 myProjMatrix;

    /// The constructed location matrix
    D3DXMATRIXA16 myLocationMatrix;
};

#endif
