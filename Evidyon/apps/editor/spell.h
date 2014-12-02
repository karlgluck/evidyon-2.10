#ifndef __SPELL_H__
#define __SPELL_H__

#include <dc/string>
#include <dc/integer>
#include "magic.h"
#include <dc/table>
#include <dc/boolean>

namespace Evidyon {
struct ClientSpellDescription;
struct ServerSpellDescription;

// A spell lets magic be used by clients in the world
class Spell : public dc::dcResource<Spell> {
public:
  Spell();
  void compileForClient(ClientSpellDescription* description) const;
  void compileForServer(ServerSpellDescription* description) const;
  bool castingNeedsTarget() const;

public:
  static std::string staticTypeName();

private:
  dc::dcString description_; // Displayed to the player to let them know what this effect does
  dc::dcInteger mp_cost_;    // How much MP it takes to cast this spell
  dc::dcTable<Magic>::Reference magic_; // Magic this spell creates when cast
  dc::dcBoolean casting_needs_target_;
};

}

#endif