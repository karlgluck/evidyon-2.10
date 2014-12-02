//------------------------------------------------------------------------------------------------
// File:    atlas.cpp
//
// Desc:    
//
// Copyright (c) 2007-2008 Unseen Studios.   All rights reserved.
//------------------------------------------------------------------------------------------------
#include "resource"
#include "stdafx.h"
#include "allgameresources.h"
#include "editresourcedialog.h"
#include "projectvolucriseditor.h"
#include <dcx/d3d>
#include "../common/isometriccamera.h"  // The common camera for viewing the world
#include <dcx/win32>
#include <dcx/eventtimer>
#include <windowsx.h>
#include <set>
#include "commctrl.h"
#include "atlasterrainmesh.h"
#include <vector>

#include "atlaseditdialog.h"
#include "../shared/server-editor/maplocationspecialfunction.h"








//------------------------------------------------------------------------------------------------
// Name:  Atlas
// Desc:  Initializes this class
//------------------------------------------------------------------------------------------------
Atlas::Atlas()
{
    member("Default Terrain - 0", &myDefaultTerrainRef[0]);
    member("Default Terrain Elevation - 0", &myDefaultTerrainElevations[0]);
    member("Default Terrain - 1", &myDefaultTerrainRef[1]);
    member("Default Terrain Elevation - 1", &myDefaultTerrainElevations[1]);
    member("Default Navigability", &myDefaultNavigability);
    member("Map Layers", &myMapLayers);
    member("Special Functions", &mySpecialFunctions);
    member("Detailed Locations", &myDetailedLocations);
    member("Swarm Spawner", &swarm_spawner_);
    member("Spawn Location", &mySpawnLocation);
    member("Spawn Radius", &mySpawnRadius);
    member("Background Sound Loop", &background_sound_loop_);
    member("Inhabitants", &inhabitants_);
}




//------------------------------------------------------------------------------------------------
// Name:  staticTypeName
// Desc:  Gets the name of this resource types
//------------------------------------------------------------------------------------------------
std::string Atlas::staticTypeName()
{
    return "Atlas";
}




//------------------------------------------------------------------------------------------------
// Name:  edit
// Desc:  Loads map components and generates a preview window
//------------------------------------------------------------------------------------------------
void Atlas::edit()
{
    AtlasEditDialog dialog(this);
    dialog.execute();
}


//------------------------------------------------------------------------------------------------
// Name:  compile
// Desc:  This method fills the output context class with data
//------------------------------------------------------------------------------------------------
bool Atlas::compile(LPDIRECT3DDEVICE9 d3dDevice, AtlasCompilationContext* context)
{
    // Check parameters
    if (APP_ERROR(!d3dDevice || !context)("Invalid parameter to Atlas::compile"))
        return false;

    // Reset context
    context->destroy();

    AtlasEditDialog dialog(this);
    dialog.compile(context);

    // Success
    return true;
}


bool Atlas::compileServerData(LPDIRECT3DDEVICE9 d3d_device,
                              dc::dcStreamOut* serverStream,
                              int map_width,
                              int map_height,
                              int monster_level_rarity_boost[RARITY_SCALE_SIZE]) {
  {
    const dc::dcGenericResource::Array& special_functions = mySpecialFunctions.getMembers();
    { // save the number of special functions
      size_t size = special_functions.size();
      serverStream->write(&size, sizeof(size));
    }
    { // write the array of special functions
      for (dc::dcGenericResource::Array::const_iterator i = special_functions.begin();
           i != special_functions.end(); ++i) {
        typedef dc::dcTable<MapLocationSpecialFunction>::Element ElementType;
        ElementType* element = reinterpret_cast<ElementType*>(*i);
        MapLocationSpecialFunction* impl = element->getImplementation();

        Evidyon::CompiledMapLocationSpecialFunction description;
        if (!impl->compileForServer(&description) ||
            !serverStream->write(&description, sizeof(description))) {
          return false;
        }
      }
    }
  }
  { // write the swarm spawning data
    CONFIRM(swarm_spawner_.compileForServer(d3d_device,
                                            map_width / Evidyon::World::REGION_SIZE,
                                            map_height / Evidyon::World::REGION_SIZE,
                                            monster_level_rarity_boost,
                                            serverStream)) else return false;
  }
  { // write the inhabitant data
    const dc::dcGenericResource::Array& inhabitants = inhabitants_.getMembers();
    { // save the number of special functions
      size_t size = inhabitants.size();
      serverStream->write(size);
    }
    { // write the array of inhabitants
      for (dc::dcGenericResource::Array::const_iterator i = inhabitants.begin();
           i != inhabitants.end(); ++i) {
        const Inhabitants::Element* element = reinterpret_cast<const Inhabitants::Element*>(*i);
        int x = element->getKey()->getX(),
            y = element->getKey()->getY();
        size_t lifeform_ai = element->getValue()->getReferencedResourceIndex();
        CONFIRM(serverStream->write(x) &&
                serverStream->write(y) &&
                serverStream->write(lifeform_ai)) else return false;
      }
    }
  }

  return true;
}
