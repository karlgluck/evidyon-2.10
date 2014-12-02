//------------------------------------------------------------------------------------------------
// File:  isometriccamera.cpp
//
// Desc:  Sets up the transform states for view rendering and manages view-based information
//
// Copyright (c) 2007-2008 Unseen Studios.   All rights reserved.
//------------------------------------------------------------------------------------------------
#include <d3dx9.h>
#include <dc/debug>
#include "isometriccamera.h"




//------------------------------------------------------------------------------------------------
// Name:  IsometricCamera
// Desc:  Resets this class
//------------------------------------------------------------------------------------------------
IsometricCamera::IsometricCamera()
{
  // Set up default information
  zero();
}




//------------------------------------------------------------------------------------------------
// Name:  setPosition
// Desc:  Sets the location of the camera on the map
//------------------------------------------------------------------------------------------------
void IsometricCamera::setPosition(float x, float z)
{
  // Build the location matrix
  D3DXMatrixTranslation(&myLocationMatrix, -x, 0.0f, -z);
}




//------------------------------------------------------------------------------------------------
// Name:  enableTopdownView
// Desc:  Changes whether or not this camera looks down on the world from directly above
//------------------------------------------------------------------------------------------------
void IsometricCamera::enableTopdownView(bool enable)
{
  myTiltWorldFlag = !enable;
}


bool IsometricCamera::topdownViewEnabled() const {
  return !myTiltWorldFlag;
}



void IsometricCamera::rebuildXenimusCamera(DWORD screen_width, DWORD screen_height) {
  const float XENIMUS_CAMERA_PARAMS[] = {
    -.70583767,
    -.16323502,
    -.076607861
  };

  // Save the screen size
  myScreenWidth = screen_width;
  myScreenHeight = screen_height;
  
  // Build an aspect ratio
  float aspect = screen_width / (float)screen_height;

  // Build the view matrix using raw data
  ZeroMemory(&myViewMatrix, sizeof(myViewMatrix));
  myViewMatrix._11 =  1.0f; //column 1 rows 0, 1, 2 are the unit vector of the X axis
  myViewMatrix._23 =  1.0f; // column 2 rows 0,1,2 are the unit vector of the y axis
  myViewMatrix._32 = -1.0f; // "" z axis
  myViewMatrix._43 =  1.0f; // must be = 1.0f
  myViewMatrix._44 =  1.0f;

  // Construct the basic view and projection matrices
  D3DXMatrixOrthoLH(&myProjMatrix, 12.0f, 12.0f, -2.0f, 10.0f);

  // This matrix is used to adjust the view
  D3DXMATRIXA16 mat;
  D3DXMatrixIdentity(&mat);

  if (myTiltWorldFlag) {
    mat._23 = XENIMUS_CAMERA_PARAMS[0];
    mat._22 = XENIMUS_CAMERA_PARAMS[1];
    mat._24 = XENIMUS_CAMERA_PARAMS[2];
  } else {
    mat._22 = -0.2f;
  }
  mat._33 = 1.0f;
  mat._44 = 1.0f;

  // Construct the view matrix
  D3DXMatrixMultiply(&myViewMatrix, &mat, &myViewMatrix);
}




//------------------------------------------------------------------------------------------------
// Name:  rebuild
// Desc:  Reconstructs the matrices
//------------------------------------------------------------------------------------------------
void IsometricCamera::rebuild(DWORD screenWidth, DWORD screenHeight, float zoom, float params[5])
{
  if (false)
  {
    // params = { -.8, -.5, -.09, 1.57, .98 };
    //const float p[] = { -.8, -.5, -.09, 1.57, .98 };
    //params = const_cast<float*>(p);

    // Save the screen size
    myScreenWidth = screenWidth;
    myScreenHeight = screenHeight;
    
    // Build an aspect ratio
    float aspect = screenWidth / (float)screenHeight;


    // Build the view matrix using raw data
    ZeroMemory(&myViewMatrix, sizeof(myViewMatrix));
    myViewMatrix._11 =  1.0f; //column 1 rows 0, 1, 2 are the unit vector of the X axis
    myViewMatrix._23 =  1.0f; // column 2 rows 0,1,2 are the unit vector of the y axis
    myViewMatrix._32 = -1.0f; // "" z axis
    myViewMatrix._43 =  1.0f; // must be > 0.0f, but has no effect until it gets really large (then starts chopping polygons)
    myViewMatrix._44 =  1.0f;

    // Construct the basic view and projection matrices
    //D3DXMatrixPerspectiveFovLH(&myProjMatrix, params[3], aspect, 0.025f + heightAdd, 25.0f + heightAdd);
    //D3DXMatrixPerspectiveOffCenterLH(&myProjMatrix, -1.0f, -1.0f, 1.0f, 1.1f, 0.025f, 100.0f);
    D3DXMatrixOrthoLH(&myProjMatrix, 12.0f, 12.0f, 0.1f, 100.0f);

     // D3DXMatrixOrthoLH(

    // This matrix is used to adjust the view
    D3DXMATRIXA16 mat;
    D3DXMatrixIdentity(&mat);

    mat._23 = params[0];
    mat._22 = params[1];
    mat._24 = params[2];
    mat._33 = 1.0f;
    mat._44 = 1.0f;

    // Construct the view matrix
    D3DXMatrixMultiply(&myViewMatrix, &mat, &myViewMatrix);
  }
  else if (true)
  {
    // Save the screen size
    myScreenWidth = screenWidth;
    myScreenHeight = screenHeight;
    
    // Build an aspect ratio
    float aspect = screenWidth / (float)screenHeight;

    // The camera vectors
    //float heightAdd = 4.625f * (zoom - 1.0f);
    float heightAdd = 0.0f;
    D3DXVECTOR3 vEyePt(0.0f, -4.625f * zoom, 0.0f);
    D3DXVECTOR3 vLookatPt(0.0f, 0.0f, 0.0f);
    D3DXVECTOR3 vUpVec(0.0f, 0.0f, -1.0f);

    // Construct the basic view and projection matrices
    D3DXMatrixLookAtLH(&myViewMatrix, &vEyePt, &vLookatPt, &vUpVec);
    D3DXMatrixPerspectiveFovLH(&myProjMatrix, D3DX_PI/2, aspect, 0.025f + heightAdd, 25.0f + heightAdd);

    // This matrix is used to adjust the view
    D3DXMATRIXA16 mat;
    D3DXMatrixIdentity(&mat);

    // Shear all of the geometry vertically if isometric view is enabled.  We do this because
    // it keeps lines parallel to the edge of the screen, whereas if we moved the view down
    // it would cause them to diverge.  The actual shearing value is arbitrily chosen.
    if (myTiltWorldFlag)
      mat._23 = -0.700f;

    // Make everything shorter and invert the view (this makes the coordinates increase to the
    // right and down along the screen)
    mat._22 = -0.2f;

    // Reduce the depth of the geometry
    mat._33 = 0.7575f;

    // Construct the view matrix
    D3DXMatrixMultiply(&myViewMatrix, &mat, &myViewMatrix);
  }
  else
  {


  // Save the screen size
  myScreenWidth = screenWidth;
  myScreenHeight = screenHeight;
  
  // Build an aspect ratio
  float aspect = screenWidth / (float)screenHeight;

  float angle = zoom;

  zoom = 4.25*2.0f*2.0f;

  float mag = 4.625f * zoom;
  

  // The camera vectors
  float heightAdd = 4.625f * (zoom - 1.0f);
  D3DXVECTOR3 vEyePt(mag * cosf(angle), -4.625f * zoom, mag * sinf(angle));
  D3DXVECTOR3 vLookatPt(0.0f, 0.0f, 0.0f);
  D3DXVECTOR3 vUpVec(0.0f, -1.0f, 0.0f);

  // Construct the basic view and projection matrices
  D3DXMatrixLookAtLH(&myViewMatrix, &vEyePt, &vLookatPt, &vUpVec);
  D3DXMatrixPerspectiveFovLH(&myProjMatrix, D3DX_PI/32, aspect, 0.025f + heightAdd, 1500.0f);

  // This matrix is used to adjust the view
  D3DXMATRIXA16 mat;
  D3DXMatrixIdentity(&mat);

  // Shear all of the geometry vertically if isometric view is enabled.  We do this because
  // it keeps lines parallel to the edge of the screen, whereas if we moved the view down
  // it would cause them to diverge.  The actual shearing value is arbitrily chosen.
  //if (myTiltWorldFlag)
  //  mat._23 = -0.700f;

  // Make everything shorter and invert the view (this makes the coordinates increase to the
  // right and down along the screen)
  //mat._22 = -0.2f;
  mat._22 = -1.0f;

  // Reduce the depth of the geometry
  //mat._33 = 0.7575f;

  // Construct the view matrix
  D3DXMatrixMultiply(&myViewMatrix, &mat, &myViewMatrix);
  }
}


//------------------------------------------------------------------------------------------------
// Name:  rebuild
// Desc:  Reconstructs the matrices
//------------------------------------------------------------------------------------------------
void IsometricCamera::rebuildAs3D(DWORD screenWidth, DWORD screenHeight, float angle, float zoom)
{
  // Save the screen size
  myScreenWidth = screenWidth;
  myScreenHeight = screenHeight;
  
  // Build an aspect ratio
  float aspect = screenWidth / (float)screenHeight;

  float zoom_internal = 4.25*2.0f*2.0f * zoom;

  float mag = 4.625f * zoom_internal;
  

  // The camera vectors
  D3DXVECTOR3 vEyePt(mag * cosf(angle), -4.625f * zoom_internal, mag * sinf(angle));
  D3DXVECTOR3 vLookatPt(0.0f, 0.0f, 0.0f);
  D3DXVECTOR3 vUpVec(0.0f, -1.0f, 0.0f);

  // Construct the basic view and projection matrices
  D3DXMatrixLookAtLH(&myViewMatrix, &vEyePt, &vLookatPt, &vUpVec);
  D3DXMatrixPerspectiveFovLH(&myProjMatrix, D3DX_PI/32, aspect, 0.025f, 1500.0f);

  // This matrix is used to adjust the view
  D3DXMATRIXA16 mat;
  D3DXMatrixIdentity(&mat);

  // Shear all of the geometry vertically if isometric view is enabled.  We do this because
  // it keeps lines parallel to the edge of the screen, whereas if we moved the view down
  // it would cause them to diverge.  The actual shearing value is arbitrily chosen.
  //if (myTiltWorldFlag)
  //  mat._23 = -0.700f;

  // Make everything shorter and invert the view (this makes the coordinates increase to the
  // right and down along the screen)
  //mat._22 = -0.2f;
  mat._22 = -1.0f;

  // Reduce the depth of the geometry
  //mat._33 = 0.7575f;

  // Construct the view matrix
  D3DXMatrixMultiply(&myViewMatrix, &mat, &myViewMatrix);
}




//------------------------------------------------------------------------------------------------
// Name:  initialize3DRendering
// Desc:  Sets up the destination device for rendering using this camera
//------------------------------------------------------------------------------------------------
bool IsometricCamera::initialize3DRendering(LPDIRECT3DDEVICE9 d3dDevice)
{
  // If the parameter is invalid, we can't do anything
  if (APP_ERROR(!d3dDevice)("Invalid device provided to IsometricCamera::initialize3DRendering"))
    return false;

  // Set up the matrices
  d3dDevice->SetTransform(D3DTS_VIEW, &myViewMatrix);
  d3dDevice->SetTransform(D3DTS_PROJECTION, &myProjMatrix);

  // Success
  return true;
}





//------------------------------------------------------------------------------------------------
// Name:  setWorldMatrix
// Desc:  Sets the designated world matrix on the device
//------------------------------------------------------------------------------------------------
bool IsometricCamera::setWorldMatrix(LPDIRECT3DDEVICE9 d3dDevice, const D3DXMATRIX* worldMatrix, DWORD index)
{
  // Check parmaeters
  if (APP_ERROR(!d3dDevice || !worldMatrix || index > 255)("Invalid parameter provided to IsometricCamera::setWorldMatrix"))
    return false;

  // Combine the matrix
  D3DXMATRIXA16 mat;
  D3DXMatrixMultiply(&mat, worldMatrix, &myLocationMatrix);
  d3dDevice->SetTransform(D3DTS_WORLDMATRIX(index), &mat);

  D3DXMatrixMultiply(&mat, &myViewMatrix, &mat);
  D3DXMatrixMultiply(&mat, &myProjMatrix, &mat);


  // Success
  return true;
}




//------------------------------------------------------------------------------------------------
// Name:  getWorldLocation
// Desc:  Ray-casts a screen coordinate into the corresponding location on the map
//------------------------------------------------------------------------------------------------
void IsometricCamera::getWorldLocation(int screenX, int screenY, float* worldX, float* worldZ)
{
  D3DXVECTOR3 vPickRayDir;
  D3DXVECTOR3 vPickRayOrig;

  // Compute the vector of the pick ray in screen space
  D3DXVECTOR3 v;
  v.x =  (((2.0f * screenX) / myScreenWidth) - 1) / myProjMatrix._11;
  v.y = -(((2.0f * screenY) / myScreenHeight) - 1) / myProjMatrix._22;
  v.z =  1.0f;

  // Get the inverse view matrix
  D3DXMATRIX mWorldView = myLocationMatrix * myViewMatrix;
  D3DXMATRIX m;
  D3DXMatrixInverse(&m, NULL, &mWorldView);

  // Transform the screen space Pick ray into 3D space
  vPickRayDir.x  = v.x*m._11 + v.y*m._21 + v.z*m._31;
  vPickRayDir.y  = v.x*m._12 + v.y*m._22 + v.z*m._32;
  vPickRayDir.z  = v.x*m._13 + v.y*m._23 + v.z*m._33;
  vPickRayOrig.x = m._41;
  vPickRayOrig.y = m._42;
  vPickRayOrig.z = m._43;

  // Hooray! We have a pick-ray; now, find the intersection with the x/z plane
  float dist = -vPickRayOrig.y / vPickRayDir.y;
  if (worldX) *worldX = vPickRayOrig.x + dist * vPickRayDir.x;
  if (worldZ) *worldZ = vPickRayOrig.z + dist * vPickRayDir.z;
}


//------------------------------------------------------------------------------------------------
// Name:  zero
// Desc:  Resets the class
//------------------------------------------------------------------------------------------------
void IsometricCamera::getScreenLocation(float worldX, float worldY, float worldZ, float* screenX, float* screenY)
{
  D3DVIEWPORT9 vpt = { 0, 0, myScreenWidth, myScreenHeight, 0.0f, 0.0f };
  D3DXVECTOR3 world(worldX, worldY, worldZ), screen;
  D3DXVec3Project(&screen, &world, &vpt, &myProjMatrix, &myViewMatrix, &myLocationMatrix);
  if (screenX) *screenX = screen.x;
  if (screenY) *screenY = screen.y;
}





//------------------------------------------------------------------------------------------------
// Name:  zero
// Desc:  Resets the class
//------------------------------------------------------------------------------------------------
void IsometricCamera::zero()
{
  myScreenWidth = myScreenHeight = 1; // Prevent divide-by-zero
  myTiltWorldFlag = true;
  D3DXMatrixIdentity(&myViewMatrix);
  D3DXMatrixIdentity(&myProjMatrix);
  D3DXMatrixIdentity(&myLocationMatrix);
}





