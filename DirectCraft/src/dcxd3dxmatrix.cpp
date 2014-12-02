//-----------------------------------------------------------------------------
//                 _,_ _, _  _, __, __, _, _    _, ___ _,_ __, _  _,  _,
//   Copyright     | | |\ | (_  |_  |_  |\ |   (_   |  | | | \ | / \ (_ 
//      (c)        | | | \| , ) |   |   | \|   , )  |  | | |_/ | \ / , )
//                 `~' ~  ~  ~  ~~~ ~~~ ~  ~    ~   ~  `~' ~   ~  ~   ~ 
//-----------------------------------------------------------------------------
#include "../dcx/dcxd3dxmatrix.h"

namespace dcx {



  
//----[  dcxD3DXMatrix  ]------------------------------------------------------
dcxD3DXMatrix::dcxD3DXMatrix() {
  member("Scaling", &scaling_);
  member("Rotation", &rotation_);
  member("Translation", &translation_); 
  scaling_.getX()->setValue(1.0f);
  scaling_.getY()->setValue(1.0f);
  scaling_.getZ()->setValue(1.0f);
}

dcxVector3<dc::dcFloat>* dcxD3DXMatrix::getScaling() { return &scaling_; }
dcxVector3<dc::dcFloat>* dcxD3DXMatrix::getRotation() { return &rotation_; }
dcxVector3<dc::dcFloat>* dcxD3DXMatrix::getTranslation() { return &translation_; }



//----[  copy  ]---------------------------------------------------------------
void dcxD3DXMatrix::copy(dcxD3DXMatrix* other) {
  scaling_.getX()->setValue(other->scaling_.getX()->getValue());
  scaling_.getY()->setValue(other->scaling_.getY()->getValue());
  scaling_.getZ()->setValue(other->scaling_.getZ()->getValue());
  rotation_.getX()->setValue(other->rotation_.getX()->getValue());
  rotation_.getY()->setValue(other->rotation_.getY()->getValue());
  rotation_.getZ()->setValue(other->rotation_.getZ()->getValue());
  translation_.getX()->setValue(other->translation_.getX()->getValue());
  translation_.getY()->setValue(other->translation_.getY()->getValue());
  translation_.getZ()->setValue(other->translation_.getZ()->getValue());
}


//----[  getTransform  ]-------------------------------------------------------
void dcxD3DXMatrix::getTransform(D3DXMATRIX* matrix) {
  D3DXMATRIX s, r, t;
  D3DXMatrixScaling(&s,
                    scaling_.getX()->getValue(),
                    scaling_.getY()->getValue(),
                    scaling_.getZ()->getValue());
  D3DXMatrixRotationYawPitchRoll(&r,
                                 D3DXToRadian(rotation_.getY()->getValue()),
                                 D3DXToRadian(rotation_.getX()->getValue()),
                                 D3DXToRadian(rotation_.getZ()->getValue()));
  D3DXMatrixTranslation(&t,
                        translation_.getX()->getValue(),
                        translation_.getY()->getValue(),
                        translation_.getZ()->getValue());
  D3DXMatrixMultiply(matrix, &s, &r);
  D3DXMatrixMultiply(matrix, matrix, &t);
}




//----[  getNormalTransform  ]-------------------------------------------------
void dcxD3DXMatrix::getNormalTransform(D3DXMATRIX* matrix) {
  D3DXMatrixRotationYawPitchRoll(matrix,
                                 D3DXToRadian(rotation_.getY()->getValue()),
                                 D3DXToRadian(rotation_.getX()->getValue()),
                                 D3DXToRadian(rotation_.getZ()->getValue()));
}



//----[  getSRTRadians  ]------------------------------------------------------
void dcxD3DXMatrix::getSRTRadians(float* sx, float* sy, float* sz,
                                  float* rx, float* ry, float* rz,
                                  float* tx, float* ty, float* tz) {
  *sx = scaling_.getX()->getValue();
  *sy = scaling_.getY()->getValue();
  *sz = scaling_.getZ()->getValue();
  *rx = D3DXToRadian(rotation_.getX()->getValue());
  *ry = D3DXToRadian(rotation_.getY()->getValue());
  *rz = D3DXToRadian(rotation_.getZ()->getValue());
  *tx = translation_.getX()->getValue();
  *ty = translation_.getY()->getValue();
  *tz = translation_.getZ()->getValue();
}



//----[  setSRTRadians  ]------------------------------------------------------
void dcxD3DXMatrix::setSRTRadians(float sx, float sy, float sz,
                                  float rx, float ry, float rz,
                                  float tx, float ty, float tz) {
  scaling_.getX()->setValue(sx);
  scaling_.getY()->setValue(sy);
  scaling_.getZ()->setValue(sz);
  rotation_.getX()->setValue(D3DXToDegree(rx));
  rotation_.getY()->setValue(D3DXToDegree(ry));
  rotation_.getZ()->setValue(D3DXToDegree(rz));
  translation_.getX()->setValue(tx);
  translation_.getY()->setValue(ty);
  translation_.getZ()->setValue(tz);
}




//----[  combineSRTRadians  ]--------------------------------------------------
void dcxD3DXMatrix::combineSRTRadians(float sx, float sy, float sz,
                                      float rx, float ry, float rz,
                                      float tx, float ty, float tz) {
  scaling_.getX()->setValue(scaling_.getX()->getValue() * sx);
  scaling_.getY()->setValue(scaling_.getY()->getValue() * sy);
  scaling_.getZ()->setValue(scaling_.getZ()->getValue() * sz);
  rotation_.getX()->setValue(rotation_.getX()->getValue() + D3DXToDegree(rx));
  rotation_.getY()->setValue(rotation_.getY()->getValue() + D3DXToDegree(ry));
  rotation_.getZ()->setValue(rotation_.getZ()->getValue() + D3DXToDegree(rz));
  translation_.getX()->setValue(translation_.getX()->getValue() + tx);
  translation_.getY()->setValue(translation_.getY()->getValue() + ty);
  translation_.getZ()->setValue(translation_.getZ()->getValue() + tz);
}


//----[  staticTypeName  ]-----------------------------------------------------
std::string dcxD3DXMatrix::staticTypeName() {
  return "D3DXMatrix";
}


}
