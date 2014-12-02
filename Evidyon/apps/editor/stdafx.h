//------------------------------------------------------------------------------------------------
// File:    stdafx.h
//
// Desc:    Header that includes symbols commonly reused by the source files of this project
//
// Copyright (c) 2007-2008 Unseen Studios.   All rights reserved.
//------------------------------------------------------------------------------------------------
#ifndef __STDAFX_H__
#define __STDAFX_H__


// Bring in OS headers
#include <windows.h>    // Main Windows header
#include <windowsx.h>   // Methods to help control Windows controls


// Link Direct3D
#include <d3dx9.h>


// Get the various DirectCraft header files used by the resources
#include <dc/debug>
#include <dc/list>
#include <dc/map>
#include <dc/rangemap>
#include <dc/table>
#include <dc/objects>
#include <dc/dcreference.h>
#include <dcx/vector3>
#include <dcx/win32>

using namespace dc;
using namespace dcx;


// Include the geometry definition header
#include "../shared/geometry.h"

#include "../shared/server-editor/server_gamefile.h"
#include "../shared/evidyon_avatar.h"
#include "../shared/evidyon_avatarinventory.h"
#include "../shared/evidyon_accounts.h"
#include "../shared/evidyon_actor.h"
#include "../shared/evidyon_enchantments.h"
#include "../shared/evidyon_magic.h"
#include "../shared/evidyon_map.h"
//#include "../shared/evidyon_network.h"
//#include "../shared/client-server/packets.h"
#include "../shared/evidyon_version.h"
#include "../shared/evidyon_world.h"
#include "../shared/client-editor/client_gamefile.h"
using namespace Evidyon;


#include "resourceactions.h"

#include "../common/unseenskinnedmesh/unseenskinnedmesh.h"
#include "unseenkeyframeanimation.h"


#define SAFE_RELEASE(p)       if (p) { p->Release(); p = NULL; }
#define SAFE_DELETE(p)        if (p) { delete p; p = NULL; } 
#define SAFE_DELETE_ARRAY(p)  if (p) { delete[] p; p = NULL; }
#define SAFE_RELEASE_ELEMENTS(a, n)   {for (size_t i=0;i<n;++i){SAFE_RELEASE(a[i])}}



#endif