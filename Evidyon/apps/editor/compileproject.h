//------------------------------------------------------------------------------------------------
// File:    compileproject.h
//
// Desc:    
//
// Copyright (c) 2007-2008 Unseen Studios.   All rights reserved.
//------------------------------------------------------------------------------------------------
#ifndef __COMPILEPROJECT_H__
#define __COMPILEPROJECT_H__


/**
 * Creates a Project Volucris data file containing all of the information the client and server
 * needs to run the game, based on the provided configuration.
 */
bool CompileProject(ProjectVolucris* pv, dc::dcStreamOut* clientStream, dc::dcStreamOut* serverStream);


#endif
