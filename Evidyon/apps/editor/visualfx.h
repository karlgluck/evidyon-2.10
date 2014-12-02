//------------------------------------------------------------------------------------------------
// File:    visualfx.h
//
// Desc:    
//
// Copyright (c) 2007-2008 Unseen Studios.   All rights reserved.
//------------------------------------------------------------------------------------------------
#ifndef __VISUALFX_H__
#define __VISUALFX_H__

namespace Evidyon {
struct CompiledVisualFXParams;
}

#include <dc/string>

class VisualFX : public dc::dcGenericResource {
public:
  // Enters data into the client effect structure
  virtual bool compileForClient(Evidyon::CompiledVisualFXParams* params) const = 0;

public:
  static std::string staticTypeName();
};




#endif