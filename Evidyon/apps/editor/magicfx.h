#ifndef __MAGICFX_H__
#define __MAGICFX_H__

#include <dc/list>
#include <dc/table>
#include "visualfx.h"
#include "wavsound.h"


namespace Evidyon {
struct CompiledMagicFXParams;
}


class MagicFX : public dc::dcResource<MagicFX> {
public:
  MagicFX();
  void compileForClient(Evidyon::CompiledMagicFXParams* compiled) const;

public:
  static std::string staticTypeName();

private:
  dc::dcTable<VisualFX>::Reference visual_fx_[3]; // 3 = const defined in compiledspecialfx.h
  dc::dcList<WAVSound>::Reference casting_sound_;
};




#endif