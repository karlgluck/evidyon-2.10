//------------------------------------------------------------------------------------------------
// Copyright (c) 2007-2008 Unseen Studios.   All rights reserved.
//------------------------------------------------------------------------------------------------
#ifndef __STDAFX_H__
#define __STDAFX_H__


#include <d3dx9.h>
#include <dc/debug>
#include <dc/filestream>

#include <set>
#include <list>
#include <string>
#include <vector>
#include <map>


#include "dxfile.h"
#include "../common/unseenskinnedmesh/unseenskinnedmesh.h"
#include "framenode.h"


#define SAFE_RELEASE( p )   if( p ) { p->Release(); p = 0; }
#define SAFE_DELETE( p )    if( p ) { delete p; p = 0; }
#define SAFE_DELETE_ARRAY( p )    if( p ) { delete[] p; p = 0; }




#endif