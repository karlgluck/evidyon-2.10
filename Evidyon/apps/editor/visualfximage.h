#ifndef __VISUALFXIMAGE_H__
#define __VISUALFXIMAGE_H__

#include <dc/table>
#include "image.h"

namespace Evidyon {
struct CompiledVisualFXImage;
}


class VisualFXImage : public dc::dcResource<VisualFXImage> {
public:
  VisualFXImage();
  void compileForClient(Evidyon::CompiledVisualFXImage* compiled) const;

public:
  static std::string staticTypeName();

private:
  dc::dcTable<Image>::Reference myImage;  /// The image for this particle
};



#endif