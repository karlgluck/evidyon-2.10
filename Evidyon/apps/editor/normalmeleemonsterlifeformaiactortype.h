#ifndef __NORMALMELEEMONSTERLIFEFORMAIACTORTYPE_H__
#define __NORMALMELEEMONSTERLIFEFORMAIACTORTYPE_H__


namespace Evidyon {
struct ClientActorDescriptionTemplate;
struct ServerActorDescriptionTemplate;
struct AIMonsterActorType;
}

#include "enchantment.h"
#include <dc/list>
#include "actortype.h"



class NormalMeleeMonsterLifeformAIActorType : public dc::dcResource<NormalMeleeMonsterLifeformAIActorType> {
public:
  NormalMeleeMonsterLifeformAIActorType();
  bool compileForServer(Evidyon::AIMonsterActorType* description);
  bool compileActorTemplate(int template_index,
                            Evidyon::ClientActorDescriptionTemplate* client_description,
                            Evidyon::ServerActorDescriptionTemplate* server_description);

public:
  static std::string staticTypeName();

private:
  dc::dcList<ActorType>::Reference actor_type_;
  dc::dcList<dc::dcReference<Enchantment>> enchantments_;

  // when compiled for the client, this is the index of this actor type's description
  int template_index_;
};



#endif