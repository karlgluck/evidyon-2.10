#ifndef __LIFEFORMAIDESCRIPTION_H__
#define __LIFEFORMAIDESCRIPTION_H__

#include "../evidyon_actor.h"
#include "../evidyon_npc.h"

namespace Evidyon {

static const int LIFEFORMAI_NORMALMELEE_ACTORTYPES = 4;
static const int LIFEFORMAI_MELEEATTACK_TYPES = 3;

struct AIMeleeAttackDescription {
  float range; // range to target within which to use this ability
  float duration; // seconds before applying damage
  ActorAction action;
  int base_damage, random_additional_damage;
  int added_damage_per_level;
};

struct AIMonsterActorType {
  unsigned int template_index;
  float size; // this is redundant--the template index references a struct with this in it
};

struct LifeformAIDescription_NormalMeleeMonster {
  AIMonsterActorType actor_types[LIFEFORMAI_NORMALMELEE_ACTORTYPES];
  AIMeleeAttackDescription melee_attack_types[LIFEFORMAI_MELEEATTACK_TYPES];
  int hp, added_hp_per_level;
  int experience;
  int added_experience_per_level;
  float movement_speed;
  float time_between_attacks;
  int treasure_class_index;
  float magic_damage_factor; // multiply spell damage by this amount
};




struct LifeformAIDescription_InteractiveNPC {
  unsigned int actor_template_index;
  NPCInteractionType interaction; // what kind of interaction this npc allows
};

struct LifeformAIDescription {
  enum Type {
    NORMAL_MELEE_MONSTER,
    INTERACTIVE_NPC,
  } type;

  union {
    LifeformAIDescription_NormalMeleeMonster normal_melee_monster;
    LifeformAIDescription_InteractiveNPC interactive_npc;
  };
};

}


#endif