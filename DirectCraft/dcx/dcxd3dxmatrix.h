//-----------------------------------------------------------------------------
//                 _,_ _, _  _, __, __, _, _    _, ___ _,_ __, _  _,  _,
//   Copyright     | | |\ | (_  |_  |_  |\ |   (_   |  | | | \ | / \ (_ 
//      (c)        | | | \| , ) |   |   | \|   , )  |  | | |_/ | \ / , )
//                 `~' ~  ~  ~  ~~~ ~~~ ~  ~    ~   ~  `~' ~   ~  ~   ~ 
//-----------------------------------------------------------------------------
#ifndef __DCXD3DXMATRIX_H__
#define __DCXD3DXMATRIX_H__

#include <d3dx9.h>
#include <dc/float>
#include "dcxvector3.h"


namespace dcx {



  
//----[  dcxD3DXMatrix  ]------------------------------------------------------
class dcxD3DXMatrix : public dcResource<dcxD3DXMatrix> {
public:
  static std::string staticTypeName();
public:
  dcxD3DXMatrix();
  void copy(dcxD3DXMatrix* other);
  dcx::dcxVector3<dc::dcFloat>* getScaling();
  dcx::dcxVector3<dc::dcFloat>* getRotation();
  dcx::dcxVector3<dc::dcFloat>* getTranslation();
  void getTransform(D3DXMATRIX* matrix);

  // Obtains the transformation matrix to use for vertex normals
  void getNormalTransform(D3DXMATRIX* matrix);

  // Obtains the values of the scaling, rotation and translation parameters.
  // The rotation values are in radians.
  void getSRTRadians(float* sx, float* sy, float* sz,
                     float* rx, float* ry, float* rz,
                     float* tx, float* ty, float* tz);

  // This method converts the rotation parameters in radians to the values
  // stored internally, which are in degrees.
  void setSRTRadians(float sx, float sy, float sz,
                     float rx, float ry, float rz,
                     float tx, float ty, float tz);

  // Multiplies the given values into this matrix.  The incoming rotations
  // are in radians.
  void combineSRTRadians(float sx, float sy, float sz,
                         float rx, float ry, float rz,
                         float tx, float ty, float tz);

protected:

  // These factors are applied in the order they are declared
  dcx::dcxVector3<dc::dcFloat> scaling_, rotation_, translation_; 
};

}



#endif