#ifndef __NORMALMELEEMONSTERLIFEFORMAI_H__
#define __NORMALMELEEMONSTERLIFEFORMAI_H__


#include <dc/float>
#include <dc/list>
#include <dc/integer>
#include <dc/string>
#include "actortype.h"
#include "lifeformai.h"
#include "aimeleeattack.h"
#include "../shared/server-editor/lifeformaidescription.h"
#include "../shared/evidyon_actor.h"
#include "normalmeleemonsterlifeformaiactortype.h"
#include "treasureclass.h"


namespace Evidyon {
struct LifeformAIDescription;
}

class NormalMeleeMonsterLifeformAI : public dc::dcResource<NormalMeleeMonsterLifeformAI, LifeformAI> {
public:
  NormalMeleeMonsterLifeformAI();
  virtual const std::string& getLifeformName() const;
  virtual bool compileActorTemplates(ActorDescriptionTemplateCompiler* compiler);
  virtual bool compileForServer(Evidyon::LifeformAIDescription* description) const;

public:
  static std::string staticTypeName();

private:
  dc::dcString name_;
  dc::dcList<NormalMeleeMonsterLifeformAIActorType> actor_types_;

  // Configures the speed of each of the different attacks.  These speeds are
  // referenced in the AIMeleeAttack class and are compiled for each template
  // description type.
  dc::dcFloat attack_durations_[Evidyon::ACTORATTACK_COUNT];

  AIMeleeAttack attack_types_[Evidyon::LIFEFORMAI_MELEEATTACK_TYPES];
  dc::dcFloat movement_speed_;
  dc::dcFloat time_between_attacks_;

  // Configures combat attributes.  The average melee/magic damage for
  // various character levels is specified in the root resource.
  dc::dcInteger intended_level_; // level characters should be when fighting this monster
  dc::dcFloat seconds_to_kill_melee_;    // seconds for an average melee-type to kill this monster
  dc::dcFloat seconds_to_kill_magic_;    // seconds for an average magic-type to kill this monster
  dc::dcFloat additional_seconds_per_level_; // how much hp. changes based on monster level

  // Configures how much experience these monsters give
  dc::dcFloat average_time_between_monsters_;
  dc::dcInteger additional_exp_per_level_; // how much exp. changes for monsters above/below intended level
  dc::dcList<TreasureClass>::Reference treasure_class_;

private:
  static dc::dcClass<NormalMeleeMonsterLifeformAI>::Implements<LifeformAI> implementsLifeformAI;
};




#endif
