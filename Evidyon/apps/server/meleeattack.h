#ifndef __MELEEATTACK_H__
#define __MELEEATTACK_H__

#include "../shared/evidyon_actor.h"

namespace Evidyon {

struct MeleeAttack {
  int attack_roll;  // based on agility, weapon skill, level
  int damage;       // raw damage to apply if it hits, based on str/weapon damage
  ActorAction swing_action; // action the attacker performs on unblocked swing
  ActorAction blocked_action; // action the attacker performs when blocked
  float duration;   // seconds this attack takes to execute
  float range;      // how far this attack reaches
  //float spread;     // area in front of the attacker affected by the attack
};

}




#endif