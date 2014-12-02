//------------------------------------------------------------------------------------------------
// Copyright (c) 2007-2008 Unseen Studios.   All rights reserved.
//------------------------------------------------------------------------------------------------
#ifndef __MAGIC_H__
#define __MAGIC_H__

#include <dc/string>
#include <dc/integer>

namespace Evidyon {

struct MagicDescription;

// This is a base class for a number of different effect types
class Magic : public dc::dcGenericResource {
public:
  Magic();

  virtual void compileForServer(MagicDescription* magic_description) = 0;

public:
  static std::string staticTypeName();

private:
};

}




#endif

