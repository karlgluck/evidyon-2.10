#include "projectile.h"
#include "../shared/server-editor/serverprojectiledescription.h"
#include "../shared/client-editor/clientprojectiledescription.h"
#include "../shared/evidyon_magic.h"



Projectile::Projectile() {
  member("Range", &range_);
  member("Carrier SpecialFX", &carrier_fx_);
  member("Carrier Sound", &carrier_sound_);
  member("Termination SpecialFX", &termination_fx_);
  member("Termination Sound", &termination_sound_);
}

void Projectile::compileForServer(Evidyon::ServerProjectileDescription* projectile_description) const {
  projectile_description->duration = range_.getValue() / MAGIC_PROJECTILE_SPEED;
}

void Projectile::compileForClient(Evidyon::ClientProjectileDescription* projectile_description) const {
  projectile_description->duration = range_.getValue() / MAGIC_PROJECTILE_SPEED;
  projectile_description->carrier_fx = carrier_fx_.getReferencedResourceIndex();
  projectile_description->termination_fx = termination_fx_.getReferencedResourceIndex();
  projectile_description->carrier_sound = carrier_sound_.getReferencedResourceIndex();
  projectile_description->termination_sound = termination_sound_.getReferencedResourceIndex();
}

std::string Projectile::staticTypeName() {
  return "Projectile";
}
