//------------------------------------------------------------------------------------------------
// File:    projectvolucris.h
//
// Desc:    The resource which contains all of the other resources that are editable in the PV
//          resource editor.  This makes it very easy to save, load, compile, merge and edit
//          an entire project.
//
// Copyright (c) 2007-2008 Unseen Studios.   All rights reserved.
//------------------------------------------------------------------------------------------------
#include "stdafx.h"
#include "allgameresources.h"



//------------------------------------------------------------------------------------------------
// Name:  ProjectVolucris
// Desc:  Initializes this class
//------------------------------------------------------------------------------------------------
ProjectVolucris::ProjectVolucris()
{
    member("Volucris", this);
    member("Resource Packs", &myResourcePacks);
    member("Font Image", &myFontImage);
    member("Title Main Image", &myTitleMainImage);
    member("Title Background Image", &myTitleBackgroundImage);
    member("GUI Image", &myGUIImage);
    member("Evidyon Symbol", &myEvidyonSymbolImage);
    member("Day/Night Light Particle", &myDayNightLightParticleImage);
    member("Images", &myImages);
    member("Textures", &myTextures);
    member("Skins", &mySkins);
    member("Meshes", &myMeshes);
    member("Scenery", &myScenery);
    member("Sounds", &myWAVSoundList);
    member("Footstep Sound", &myFootstepSound);
    member("Button Sound", &myButtonSound);
    member("Skinned Meshes", &mySkinnedMeshes);
    member("Skinned Mesh Animations", &mySkinnedMeshAnimations);
    member("Terrain", &myTerrain);
    member("Terrain Rules", &myTerrainRules);
    member("Elevations", &myElevations);
    member("VisualFX Images", &myVisualFXImages);
    member("VisualFX", &myVisualFX);
    member("MagicFX", &myMagicFX);
    member("Physical Hit Magic FX", &myPhysicalHitMagicFX);
    member("Spawn Magic FX", &mySpawnMagicFX);
    member("Projectiles", &myProjectiles);
    member("Enchantments", &myEnchantments);
    member("Magic", &myMagic);
    member("Spells", &mySpells);
    member("Items", &myItems);
    member("Actor Types", &myActorTypes);
    member("Treasure Generation", &treasure_generation_);
    member("Lifeform AI", &myLifeformAITable);
    member("World Atlases", &myAtlases);
    member("Credits", &myCredits);
    member("Races", &avatar_race_list_);
    member("Classes", &avatar_class_list_);
    member("Level Definitions", &myLevelDefinitions);
}


//------------------------------------------------------------------------------------------------
// Name:  staticTypeName
// Desc:  The name of this resource type
//------------------------------------------------------------------------------------------------
std::string ProjectVolucris::staticTypeName()
{
    return "Project Volucris";
}


//------------------------------------------------------------------------------------------------
// Name:  clear
// Desc:  Erases all of the information in this structure
//------------------------------------------------------------------------------------------------
void ProjectVolucris::clear()
{
    myResourcePacks.clear();
    myFontImage.setGenericReferencedResource(NULL);
    myGUIImage.setGenericReferencedResource(NULL);
    myTitleMainImage.setGenericReferencedResource(NULL);
    myTitleBackgroundImage.setGenericReferencedResource(NULL);
    myEvidyonSymbolImage.setGenericReferencedResource(NULL);
    myDayNightLightParticleImage.setGenericReferencedResource(NULL);
    myImages.clear();
    myTextures.clear();
    mySkins.clear();
    myMeshes.clear();
    myScenery.clear();
    myTerrain.clear();
    myTerrainRules.clear();
    myCredits.clear();
    myTerrain.clear();
    myElevations.clear();
    myAtlases.clear();
    myVisualFXImages.clear();
    myMagicFX.clear();
    myVisualFX.clear();
    myMagic.clear();
    mySpells.clear();
    myProjectiles.clear();
    myEnchantments.reset();
    mySkinnedMeshes.clear();
    mySkinnedMeshAnimations.clear();
    myActorTypes.clear();
    //avatar_race_list_.clear();
    //avatar_class_list_.clear();
    myItems.clear();
    myCredits.clear();
    myItems.clear();
    myLifeformAITable.clear();
    myWAVSoundList.clear();
}



void ProjectVolucris::getReservedNames(std::list<std::string>* names) {
  {
    const dcGenericResource::Array& races = avatar_race_list_.getMembers();
    for (dcGenericResource::Array::const_iterator i = races.begin(); i != races.end(); ++i) {
      names->push_back((*i)->getName());
    }
  }
  {
    const dcGenericResource::Array& classes = avatar_class_list_.getMembers();
    for (dcGenericResource::Array::const_iterator i= classes.begin(); i != classes.end(); ++i) {
      names->push_back((*i)->getName());
    }
  }
  {
    const dcGenericResource::Array& lifeforms = myLifeformAITable.getMembers();
    for (dcGenericResource::Array::const_iterator i = lifeforms.begin(); i != lifeforms.end(); ++i) {
      dc::dcTable<LifeformAI>::Element* lifeform_ai =
        (dc::dcTable<LifeformAI>::Element*)(*i);
      names->push_back(lifeform_ai->getImplementation()->getLifeformName());
    }
  }

}