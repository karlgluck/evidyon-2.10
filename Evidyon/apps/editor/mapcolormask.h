#ifndef __MAPCOLORMASK_H__
#define __MAPCOLORMASK_H__



#include <dc/string>
#include <dc/filename>
#include <dcx/vector3>
#include <dc/byte>
#include <d3dx9.h>
#include <dc/list>
#include <dcx/d3dcolor>
#include <vector>



// These methods are helpful for loading mask files

bool GetMapMaskInfo(const char* file_name,
                    unsigned int* width,
                    unsigned int* height);

bool LoadMapMaskData(LPDIRECT3DDEVICE9 d3d_device,
                     const char* file_name,
                     unsigned int width,
                     unsigned int height,
                     D3DCOLOR* data_buffer);

bool AcquireMapMask(LPDIRECT3DDEVICE9 d3d_device,
                    const char* file_name,
                    unsigned int* width_out,
                    unsigned int* height_out,
                    D3DCOLOR** data_buffer);

void ReleaseMapMask(D3DCOLOR* data_buffer);


typedef std::vector<D3DCOLOR> MaskColorTypes;
bool AcquireMapToTypeIndexMask(LPDIRECT3DDEVICE9 d3d_device,
                               const char* file_name,
                               const MaskColorTypes& mask_color_types,
                               unsigned int* width_out,
                               unsigned int* height_out,
                               int** map_out);

void ReleaseMapToTypeIndexMask(int* indexed_map);



template <typename MappedType> class MapColorMask : public dc::dcResource<MapColorMask<MappedType>> {
  static const char* ELEMENT_DATA_MEMBER_NAME;
  static const char* ELEMENT_LIST_NAME;

public:
  // The type that is stored in the internal list
  class Element : public dc::dcResource<Element> {
  public:
    Element() {
      member("Color", &color_);
      member(ELEMENT_DATA_MEMBER_NAME, &data_);
    }

    D3DCOLOR getMaskD3DColor() const {
      return color_.getD3DColor();
    }

    void setMaskD3DColor(D3DCOLOR color) {
      color_.setD3DColor(color);
    }

    COLORREF getMaskRGBColor() const {
      return color_.getRGBColor();
    }

    void setMaskRGBColor(COLORREF color) {
      color_.setRGBColor(color);
    }

    const MappedType& getData() const {
      return data_;
    }

    MappedType& getData() {
      return data_;
    }

  public:
    static std::string staticTypeName() {
      return std::string("MapColorMask Element of ") + MappedType::staticTypeName();
    }

  private:
    dcx::dcxD3DColor color_;
    MappedType data_;
  };

public:
  MapColorMask() {
    member("Mask Image", &mask_image_);
    member(ELEMENT_LIST_NAME, &elements_);
  }

  const std::string& getMaskImageFileName() const {
    return mask_image_.getValue();
  }

  const dc::dcGenericResource::Array& getElementsList() const {
    return elements_.getMembers();
  }

  void addColor(const std::string& name, D3DCOLOR color) {
    Element* element = (Element*)elements_.generateElement(name);
    element->setMaskD3DColor(color);
  }

public:
  static std::string staticTypeName() {
    return std::string("MapColorMask of ") + MappedType::staticTypeName();
  }

private:
  dc::dcFileName mask_image_; // The image file to use as a mask
  dc::dcList<Element> elements_;  // What the different colors in the source map mean
};



#endif