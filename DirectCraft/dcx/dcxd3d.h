//------------------------------------------------------------------------------------------------
// File:    dcxd3d.h
//
// Desc:    Utility functions to ease interaction with Direct3D on the Windows platform
//
// Copyright (c) 2007-2008 Unseen Studios.   All rights reserved.
//------------------------------------------------------------------------------------------------
#ifndef __DCXD3D_H__
#define __DCXD3D_H__

namespace dcx {

    /**
     * This function is intended for use when one wants to use an existing Direct3D device on a
     * new window.  This function generates the parameters automatically (using default values)
     * and creates a new swap chain on the device.  After this method, the swap chain must be
     * activated as the render target by dcxD3DSetTargetSwapChain.
     * @note The source device must have a rendering area at least as large as that of this
     *       device for the rendering to show up correctly; otherwise, rendering will occur
     *       only in a section of the window.
     * @param d3dDevice The source device
     * @param targetWindow The window the swap chain should render into
     * @param newSwapChain Destination for the new rendering interface pointer
     * @param d3dppOut Direct3D device parameter output; optional
     * @param setAsTarget Whether to immediately set the device to render into the created chain
     */
    bool dcxD3DDuplicateDeviceForWindow( LPDIRECT3DDEVICE9 d3dDevice, HWND targetWindow,
                                         LPDIRECT3DSWAPCHAIN9* newSwapChain,
                                         D3DPRESENT_PARAMETERS* d3dppOut = 0 );

    /**
     * Sets up the Direct3D device associated with the provided swap chain to render into
     * that chain's window.  This must be done outside of BeginScene/EndScene pairs.  This
     * method automatically creates and sets a new perspective matrix that is valid for the
     * swap chain.
     * @param swapChain The target swap chain for rendering
     * @param zNear The view frustum's near z-value
     * @param zFar The view frustum's far z-value
     * @param fov The y-axis (horizontal) field of view for the perspective matrix
     * @return Whether or not the method succeeded
     */
    bool dcxD3DSetTargetSwapChain( LPDIRECT3DSWAPCHAIN9 swapChain,
                                   float zNear, float zFar,
                                   float fov = D3DX_PI / 4.0f );


}


#endif