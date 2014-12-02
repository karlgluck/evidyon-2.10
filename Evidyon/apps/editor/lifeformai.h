#ifndef __LIFEFORMAI_H__
#define __LIFEFORMAI_H__



namespace Evidyon {
struct LifeformAIDescription;
struct ClientActorDescriptionTemplate;
}

class ActorDescriptionTemplateCompiler;

#include <dc/string>

class LifeformAI : public dc::dcGenericResource {
public:
  virtual const std::string& getLifeformName() const = 0;

  // Writes out each of the lifeform's actor description templates.
  virtual bool compileActorTemplates(ActorDescriptionTemplateCompiler* compiler) = 0;
  virtual bool compileForServer(Evidyon::LifeformAIDescription* description) const = 0;

public:
  static std::string staticTypeName();

private:
};




#endif