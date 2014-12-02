#include "magicfx.h"
#include "../shared/compiledspecialfx.h"


MagicFX::MagicFX() {
  member("Visual FX - 1", &visual_fx_[0]);
  member("Visual FX - 2", &visual_fx_[1]);
  member("Visual FX - 3", &visual_fx_[2]);
  member("Casting Sound", &casting_sound_);
}


void MagicFX::compileForClient(Evidyon::CompiledMagicFXParams* compiled) const {
  compiled->sound_fx_index = casting_sound_.getReferencedResourceIndex();
  for (int i = 0; i < Evidyon::CompiledMagicFXParams::MAX_VISUAL_FX; ++i) {
    const dc::dcTable<VisualFX>::Element* element = visual_fx_[i].getReferencedResource();
    compiled->visual_fx_indices[i] =
        element == NULL ? 0xFFFFFFFF : element->getIndex();
  }
}


std::string MagicFX::staticTypeName() {
  return "MagicFX";
}
