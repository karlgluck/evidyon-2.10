#include "visualfximage.h"
#include "../shared/compiledspecialfx.h"


VisualFXImage::VisualFXImage() {
  member("Image", &myImage);
}


void VisualFXImage::compileForClient(Evidyon::CompiledVisualFXImage* compiled) const {
  const dc::dcTable<Image>::Element* element = myImage.getReferencedResource();
  compiled->image_index =
      element == NULL ? 0xFFFFFFFF : element->getIndex();
}


std::string VisualFXImage::staticTypeName() {
  return "VisualFXImage";
}

