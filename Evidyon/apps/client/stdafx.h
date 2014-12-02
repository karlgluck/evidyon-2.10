//------------------------------------------------------------------------------------------------
// File:  stdafx.h
//
// Desc:  Standard include file for all source files in the client applicatio
//
// Copyright (c) 2007-2008 Unseen Studios.   All rights reserved.
//------------------------------------------------------------------------------------------------
#ifndef __STDAFX_H__
#define __STDAFX_H__

#pragma warning(disable : 4530)
#pragma warning(disable : 4355)

#pragma warning(error : 4800)
#pragma warning(error : 4804)
// unreferenced local variable
#pragma warning(error : 4101)
#pragma warning(error : 4146)

// check operator precedence for possible error; use parentheses to clarify precedence
#pragma warning(error : 4554)

#include <enet/enet.h>
#include <dcx/win32>
#include <dcx/inifilereader>
#include <dcx/statemachine>
#include <dcx/memorypool>

#include <list>

//using namespace dc;
using namespace dcx;

#include <d3dx9.h>
#include <stdio.h>
#include "cleanup.h"
#include <string>
#include <dc/debug>
#include <map>
#include <dsound.h>
#include "../common/promotedwarnings.h"



#define PROJECTILE_WEAPON_ATTACK_RANGE    6.0f
#define PROJECTILE_WEAPON_ATTACK_RANGE_SQ   (PROJECTILE_WEAPON_ATTACK_RANGE*PROJECTILE_WEAPON_ATTACK_RANGE)

namespace Evidyon {




#pragma pack(push, 1)

struct ItemForTrade {
  unsigned listing_id  : 32;
  unsigned price     : 32;
  unsigned quantity  : 8;
  unsigned type    : 15;
  unsigned purchased   : 1;
  unsigned you_sell  : 1; // only valid for "get listed items" by client; denotes that the viewer is the seller
  unsigned __void    : 7;
};
}

namespace Economy
{
static const unsigned int MAX_ITEMS_LISTED_PER_CHARACTER = 16;
static const unsigned int MAX_ITEMS_PER_BAZAAR_PAGE = 256/sizeof(Evidyon::ItemForTrade);
}



#pragma pack(pop)



namespace Economy {

extern const unsigned int BAZAAR_LISTING_TAX_SHIFT; // take 1/(1>>LISTING_TAX_SHIFT) of the listing price as the cost
extern const unsigned int BAZAAR_MINIMUM_PRICE_TO_TAX;  // don't tax listings under this amount


/**
 * Calculates how much money must be spent to list items in the bazaar.  Cost is a positive number.
 */
bool getCostToListItemsInBazaar(unsigned int* prices,
                unsigned int number_of_prices,
                int* cost);

}
/*
#include "../common/volucrisnetwork.h"
#include "../shared/volucrisclientnetwork.h"
#include "../shared/clientgamefile.h"*/

using namespace Evidyon;
#include "../shared/evidyon_avatarinventory.h"
#include "../shared/evidyon_accounts.h"
#include "../shared/evidyon_actor.h"
#include "../shared/evidyon_avatar.h"
#include "../shared/evidyon_enchantments.h"
#include "../shared/evidyon_map.h"
#include "../shared/evidyon_network.h"
#include "../shared/client-server/packets.h"
#include "../shared/evidyon_version.h"
#include "../shared/evidyon_world.h"
#include "../shared/client-editor/client_gamefile.h"
#include "../shared/client-editor/clientspelldescription.h"

#include "encodepackets.h"
#include "decodepackets.h"

#include "../shared/compiledatlasformat.h"
#include "../shared/geometry.h"
#include "../common/isometriccamera.h"

// Include generic GUI implementations
#include "../common/GUI/guipoint.h"
#include "../common/GUI/guisize.h"
#include "../common/GUI/guirect.h"
#include "../common/GUI/guicanvas.h"
#include "../common/GUI/guielement.h"
#include "../common/GUI/guivertex.h"
#include "../common/GUI/guiindex.h"
#include "../common/GUI/guilayer.h"
#include "../common/GUI/guifont.h"
#include "../common/GUI/guiquad.h"

#include "../common/GUI/guistretchfillelement.h"
#include "../common/GUI/guiborderedfillelement.h"
#include "../common/GUI/guipictureelement.h"
#include "../common/GUI/guitextelement.h"
#include "../common/GUI/guitextlabelelement.h"

#include "../common/GUI/guimouse.h"
#include "../common/GUI/guikeyboard.h"

#include "../common/GUI/guicomponent.h"
#include "../common/GUI/guibutton.h"
#include "../common/GUI/guislider.h"
#include "../common/GUI/guieditabletextline.h"
#include "../common/GUI/guilist.h"

//#include "../common/GUI/guimodule.h"

#include "../common/GUI/guilayerbasic.h"

#include "useractionqueue.h"

// Include Volucris-specific GUI implementations
#include "volucrisguisideiconselement.h"
#include "volucrisguibutton.h"
#include "volucrisguititledpanel.h"
#include "volucrisguiitemdescriptionpanel.h"
#include "volucrisguilistentrybackground.h"
#include "volucrisguilistentryicon.h"
#include "volucrisguilistentrytext.h"
#include "volucrisguislider.h"
#include "volucrisguilist.h"
#include "volucrisguiitemlist.h"
#include "volucrisguikeyboardinputsource.h"
#include "volucrisguimouseinputsource.h"
#include "volucrisguiselectabletextlinelist.h"
#include "volucrisguiinventoryitemlist.h"
#include "volucrisguiinventorydisplay.h"
#include "volucrisguieditabletextline.h"
#include "volucrisguieffectkeybindinglist.h"
#include "volucrisguiuseractionkeybindinglist.h"
#include "volucrisguikeybindingscreen.h"
#include "volucrisguistatdisplay.h"
#include "volucrisguichatlog.h"
#include "volucrisguidropitemsscreen.h"
#include "volucrisguidefs.h"
#include "bazaardisplay.h"
#include "chattextline.h"

#include "../common/unseenskinnedmesh/unseenskinnedmesh.h"


#include "win32buttonmanager.h"
#include "spritemanager.h"
#include "gamefiledata.h"
#include "animatedmeshcontroller.h"
#include "sceneryrenderer.h"
#include "volucrisclient.h"
#include "strings.h"
#include "winmain.h"


#define SAFE_RELEASE(p)     if (p) { (p)->Release(); (p) = NULL; }
#define SAFE_DELETE(p)    if (p) { delete (p); (p) = NULL; } 
#define SAFE_DELETE_ARRAY(p)  if (p) { delete[] (p); (p) = NULL; }
#define SAFE_RELEASE_ELEMENTS(a, n)   if (a){for (size_t i=0;i<n;++i){SAFE_RELEASE(a[i])}}



#endif