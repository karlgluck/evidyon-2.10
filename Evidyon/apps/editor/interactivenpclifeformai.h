#ifndef __INTERACTIVENPCLIFEFORMAI_H__
#define __INTERACTIVENPCLIFEFORMAI_H__

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



class InteractiveNPCLifeformAI : public dc::dcResource<InteractiveNPCLifeformAI, LifeformAI> {
public:
  InteractiveNPCLifeformAI();
  virtual const std::string& getLifeformName() const;
  virtual bool compileActorTemplates(ActorDescriptionTemplateCompiler* compiler);
  virtual bool compileForServer(Evidyon::LifeformAIDescription* description) const;

public:
  static std::string staticTypeName();

private:
  dc::dcString name_;
  dc::dcList<ActorType>::Reference actor_type_;
  dc::dcEnum<Evidyon::NPCInteractionType> interaction_type_;
  dc::dcBoolean wander_; // move around?
  dc::dcList<Item>::Reference equipped_items_[Evidyon::ACTORATTACHMENTPOINT_COUNT];

  // dc::dcList<Quest>::Reference quests_;
  // dc::dcList<Item>::Reference for_sale_;
  // dc::dcList<Item>::Reference buys_;

private:
  unsigned int compiled_actor_template_index_;

private:
  static dc::dcClass<InteractiveNPCLifeformAI>::Implements<LifeformAI> implementsLifeformAI;
};



#endif