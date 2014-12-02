#ifndef __EVIDYON_ACTOR_H__
#define __EVIDYON_ACTOR_H__

namespace Evidyon {



typedef unsigned int ActorTypeIndex;

enum ActorAttachmentPoint {
  ACTORATTACHMENTPOINT_HELMET,
  ACTORATTACHMENTPOINT_WEAPON,
  ACTORATTACHMENTPOINT_SHIELD,

  ACTORATTACHMENTPOINT_COUNT,
  ACTORATTACHMENTPOINT_INVALID,
};


enum CorporealState {
  DEAD,
  NORMAL,
  IN_COMBAT,
  SPIRIT_FORM,
  CORPOREALSTATE_COUNT,
  INVALID,
};


enum ActorAttack {
  ACTORATTACK_1,
  ACTORATTACK_2,
  ACTORATTACK_3,
  ACTORATTACK_4,

  ACTORATTACK_COUNT,
};

// These are different sounds that actors can make
// due to an action.
// Shared editor->client
enum ActorSound {
  ACTORSOUND_ATTACK1,
  ACTORSOUND_ATTACK2,
  ACTORSOUND_DIE1,
  ACTORSOUND_DIE2,
  ACTORSOUND_GOTHIT1,
  ACTORSOUND_GOTHIT2,
  ACTORSOUND_COUNT,
  ACTORSOUND_INVALID,
};

/**
 * Different things an actor does
 */
enum ActorAction
{
  ACTORACTION_ATTACK1, // uses actor sound 1
  ACTORACTION_ATTACK2, // uses sound 1
  ACTORACTION_ATTACK3, // uses sound 2
  ACTORACTION_ATTACK4, // uses sound 2
  ACTORACTION_BLOCK1,
  ACTORACTION_BLOCK2,
  ACTORACTION_BLOCK3,
  ACTORACTION_CASTSPELL1,
  ACTORACTION_CASTSPELL2,
  ACTORACTION_GOTHIT,
  ACTORACTION_DIE,

  ACTORACTION_NONE,

  ACTORACTION_INVALID,

  ACTORACTION_COUNT = ACTORACTION_NONE,

  // Actions whose speed varies depending on the attributes of the character.
  // The speed values are used to modify how quickly the animation plays.
  ACTORACTION_VARIABLE_SPEED_COUNT = ACTORACTION_GOTHIT,

};

#if (ACTORACTION_COUNT >= (1<<4))
#error Too many actor actions to fit into an actor sync packet!
#endif

}

#endif