#ifndef __MAPLOCATIONSPECIALFUNCTION_H__
#define __MAPLOCATIONSPECIALFUNCTION_H__

#include <dc/string> // for dc::dcGenericResource/std::string

namespace Evidyon {

struct CompiledMapLocationSpecialFunction;

}

class MapLocationSpecialFunction : public dc::dcGenericResource {
public:
  virtual bool compileForServer(Evidyon::CompiledMapLocationSpecialFunction* description) = 0;

public:
  static std::string staticTypeName();
};





#endif