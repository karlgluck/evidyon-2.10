//------------------------------------------------------------------------------------------------
// Copyright (c) 2007-2008 Unseen Studios.   All rights reserved.
//------------------------------------------------------------------------------------------------
#ifndef __PROJECTILE_H__
#define __PROJECTILE_H__

#include <dc/float>
#include <dc/table>
#include <dc/list>
#include "visualfx.h"
#include "wavsound.h"

namespace Evidyon {

struct ServerProjectileDescription;
struct ClientProjectileDescription;

}


// Defines a type of projectile
class Projectile : public dc::dcResource<Projectile> {
public:
  Projectile();
  void compileForServer(Evidyon::ServerProjectileDescription* projectile_description) const;
  void compileForClient(Evidyon::ClientProjectileDescription* projectile_description) const;

public:
  static std::string staticTypeName();

private:
  dc::dcFloat range_;
  dc::dcFloat speed_;
  dc::dcTable<VisualFX>::Reference carrier_fx_, termination_fx_;
  dc::dcList<WAVSound>::Reference carrier_sound_, termination_sound_;
};






#endif

