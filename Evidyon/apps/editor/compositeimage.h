//#ifndef __COMPOSITEIMAGE_H__
//#define __COMPOSITEIMAGE_H__
//
//#include <dc/interface>
//#include <dc/integer>
//#include <dc/filename>
//#include "image.h"
//#include "mapcolormask.h"
//
//
//class CompositeImage : public dc::dcResource<CompositeImage,Image>{
//public:
//  CompositeImage();
//  virtual bool obtainD3DTexture(LPDIRECT3DDEVICE9 pd3dDevice,
//                                DWORD options,
//                                LPDIRECT3DTEXTURE9* ppd3dTexture) const;
//
//public:
//  static std::string staticTypeName();
//
//private:
//  MapColorMask<dc::dcFileName> images_to_stack_;
//  dc::dcInteger subimage_columns_;
//  dc::dcInteger subimage_rows_;
//  dc::dcInteger subimage_index_;
//
//private:
//  static dc::dcClass<CompositeImage>::Implements<Image> implementsImage;
//};
//
//
//
//#endif