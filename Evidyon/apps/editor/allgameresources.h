//------------------------------------------------------------------------------------------------
// File:    allgameresources.h
//
// Desc:    Includes all of the game resources used in the editor.  This isn't a part of stdafx.h
//          because any one change in a single header will cause a complete recompile.  Some files
//          need to know about every resource, though (like the resource actions file and compile
//          method declarations file) so this file is still useful in some regard.
//
// Copyright (c) 2007-2008 Unseen Studios.   All rights reserved.
//------------------------------------------------------------------------------------------------
#ifndef __ALLGAMERESOURCES_H__
#define __ALLGAMERESOURCES_H__

// Declare types
class ProjectVolucris;


#include "magic.h"
#include "detailedmagic.h"


#include "colorkey.h"

#include "image.h"
#include "d3dximage.h"

#include "texture.h"
#include "animatedtexture.h"
#include "circlingtexture.h"
#include "slidingtexture.h"
#include "statictexture.h"

#include "skin.h"

#include "mesh.h"
#include "xmesh.h"
#include "mdjmesh.h"


#include "scenery.h"

#include "location.h"

#include "terrain.h"
#include <dc/table>

#include "terrainrule.h"

#include "../shared/compiledatlasformat.h"

#include "maplayer.h"

#include "atlasterrainmesh.h"
#include <dcx/win32>
#include <dcx/d3d>
#include "../common/isometriccamera.h"  // The common camera for viewing the world
#include "atlas.h"
#include "atlaseditdialog.h"
#include "atlascompilationcontext.h"


#include "contributor.h"

#include "resourcepack.h"
#include "packimage.h"
#include "packmesh.h"


#include "wavsound.h"


//#include "particleswirleffectvisualization.h"
//#include "particletraileffectvisualization.h"
//#include "beameffectvisualization.h"
//#include "sceneryeffectvisualization.h"

#include "visualfximage.h"
#include "visualfx.h"
#include "magicfx.h"
#include "visualfx_orbitingparticles.h"
#include "visualfx_particletrail.h"

#include "avatarmodifier.h"

#include "item.h"

#include "unseenskinnedmeshresource.h"
#include "unseenkeyframeanimation.h"
#include "skinnedmeshattachmentpoint.h"
#include "actortype.h"

#include "aicharactertype.h"

//#include "monsterloot.h"
//#include "monsterclass.h"
//#include "monsterspawn.h"

#include "playercharacterrace.h"
#include "playercharacterclasslevelspells.h"
#include "playercharacterclass.h"

#include "leveldefinitions.h"

#include "projectile.h"

#include "enchantment.h"
#include "enchantmentclass.h"
#include "enchantmentclassimpl.h"
#include "enchantmentclasslist.h"

#include "lifeformai.h"


// This one is special--it is the main resource that contains everything in the editor!
#include "projectvolucris.h"



#endif