#include "globalgamefilemanager.h"
#include "../shared/server-editor/server_gamefile.h"
#include "../common/null.h"
#include <dc/debug>
#include "../shared/server-editor/serveratlas.h"
#include "../shared/evidyon_avatar.h"
#include "../shared/server-editor/magicdescription.h"
#include "../shared/server-editor/serverspelldescription.h"
#include "../common/safemacros.h"
#include "../shared/server-editor/serverprojectiledescription.h"
#include "globalworldmanager.h"
#include "../shared/server-editor/maplocationspecialfunction.h"

#include "..\shared\server-editor\swarmspawndescription.h"

#include "globallifeformaimanager.h"
#include "globalactormanager.h"
#include "../shared/actordescriptiontemplate.h"
#include "globalitemmanager.h"

#include "../shared/server-editor/evidyon_gameplay_design.h"

#include "avatarattributes.h"
#include "uniqueavatarnamechecker.h"


namespace Evidyon {


GlobalGamefileManager* GlobalGamefileManager::singleton_ = NULL;


GlobalGamefileManager* GlobalGamefileManager::singleton() {
  return singleton_;
}

bool GlobalGamefileManager::create(const char* source_file,
                                   UniqueAvatarNameChecker* unique_avatar_name_checker,
                                   GlobalItemManager* item_manager,
                                   GlobalActorManager* actor_manager,
                                   GlobalWorldManager* world_manager,
                                   GlobalLifeformAIManager* lifeform_ai_manager) {
  // TODO: this is a prime candidate area for a Cleanup class
  spell_descriptions_ = NULL;
  magic_descriptions_ = NULL;
  myPlayerRaceArray = NULL;
  myPlayerClassArray = NULL;
  projectile_descriptions_ = NULL;

  FILE* fp;
  if (APP_ERROR(0 != fopen_s(&fp, source_file, "rb"))("Couldn't open server game file"))
    goto STARTUP_FAILED;


  //==============================================================================================
  // Reserved Names
  //==============================================================================================
  {
    size_t size = 0;
    fread(&size, sizeof(size), 1, fp);
    for (size_t s = 0; s < size; ++s) {
      size_t length = 0;
      fread(&length, sizeof(length), 1, fp);
      char* buffer = new char[length+1];
      fread(buffer, sizeof(char), length, fp);
      buffer[length] = '\0';
      unique_avatar_name_checker->addName(buffer);
      delete[] buffer;
    }
  }


  //==============================================================================================
  // Races
  //==============================================================================================

  // Read the number of races
  if (APP_ERROR(!fread(&myPlayerRaceArraySize, sizeof(myPlayerRaceArraySize), 1, fp))("Couldn't read race array size") ||
    APP_FATAL(!(myPlayerRaceArray = new AvatarRaceDescription[myPlayerRaceArraySize]))("Couldn't allocate race properties"))
    goto STARTUP_FAILED;

  // Read each of the races
  for (size_t i = 0; i < myPlayerRaceArraySize; ++i)
  {
    unsigned char level1AttributeMaximums[5];
    if (APP_ERROR(!fread(level1AttributeMaximums, sizeof(unsigned char), 5, fp))("Couldn't read race information"))
      goto STARTUP_FAILED;

    // Assign the maxima
    for (int j = 0; j < 5; ++j)
      myPlayerRaceArray[i].level1AttributeMaximums[j] = (int)level1AttributeMaximums[j];

    // Load the actor types for each of the race/gender combinations
    if (APP_ERROR(!fread(&myPlayerRaceArray[i].male_actor_template_index, sizeof(int), 1, fp) ||
                  !fread(&myPlayerRaceArray[i].female_actor_template_index, sizeof(int), 1, fp))("Couldn't read actor index"))
      goto STARTUP_FAILED;

    // Read hair styles
    if (!fread(myPlayerRaceArray[i].hair_styles,
               sizeof(int),
               Evidyon::AVATARGENDER_COUNT * Evidyon::AVATAR_RACE_HAIR_STYLES_PER_GENDER,
               fp)) goto STARTUP_FAILED;
  }



  //==============================================================================================
  // Classes
  //==============================================================================================
  if (APP_ERROR(!fread(&myPlayerClassArraySize, sizeof(myPlayerClassArraySize), 1, fp))("Couldn't read class array size") ||
    APP_FATAL(!(myPlayerClassArray = new AvatarClassDescription[myPlayerClassArraySize]))("Couldn't allocate class properties"))
    goto STARTUP_FAILED;

  // Read all of the classes
  if (APP_ERROR(!fread(myPlayerClassArray, sizeof(AvatarClassDescription), myPlayerClassArraySize, fp))("Couldn't read class information"))
    goto STARTUP_FAILED;


  //==============================================================================================
  // Level-up experience
  //==============================================================================================
  if (APP_ERROR(!fread(myLevelUpExperience, sizeof(int), HIGHEST_DEFINED_LEVEL, fp))("Couldn't read level up experience"))
    goto STARTUP_FAILED;


  //==============================================================================================
  // Projectile Descriptions
  //==============================================================================================
  {
    size_t number;
    if (APP_ERROR(!fread(&number, sizeof(number), 1, fp))("Couldn't read magic array size") ||
      APP_FATAL(!(projectile_descriptions_ = new ServerProjectileDescription[number]))("Couldn't allocate projectile descriptions") ||
      APP_ERROR(!fread(projectile_descriptions_, sizeof(ServerProjectileDescription), number, fp))(""))
      goto STARTUP_FAILED;
    number_of_projectile_descriptions_ = number;
  }


  //==============================================================================================
  // Magic Descriptions
  //==============================================================================================
  {
    size_t number;
    if (APP_ERROR(!fread(&number, sizeof(number), 1, fp))("Couldn't read magic array size") ||
      APP_FATAL(!(magic_descriptions_ = new MagicDescription[number]))("Couldn't allocate magic properties") ||
      APP_ERROR(!fread(magic_descriptions_, sizeof(MagicDescription), number, fp))(""))
      goto STARTUP_FAILED;
    number_of_magic_descriptions_ = number;
  }


  //==============================================================================================
  // Spell Descriptions
  //==============================================================================================
  {
    size_t number;
    if (APP_ERROR(!fread(&number, sizeof(number), 1, fp))("Couldn't read spell array size") ||
      APP_FATAL(!(spell_descriptions_ = new SpellDescription[number]))("Couldn't allocate spell properties"))
      goto STARTUP_FAILED;

    for (size_t i = 0; i < number; ++i) {
      ServerSpellDescription description;
      fread(&description, sizeof(description), 1, fp);
      spell_descriptions_[i].magic = description.magic_index < number_of_magic_descriptions_ ?
      &magic_descriptions_[description.magic_index] : NULL;
      spell_descriptions_[i].mp_cost = description.mp_cost;
    }
    number_of_spell_descriptions_ = number;
  }

  // Get defined effect types
  fread(&spawn_magic_fx_, sizeof(spawn_magic_fx_), 1, fp);

  //==============================================================================================
  // Item Description & Treasure
  //==============================================================================================
  CONFIRM(item_manager->create(fp)) else return false;


  //==============================================================================================
  // AI Lifeforms
  //==============================================================================================
  {
    ASSERT(lifeform_ai_manager->create(fp)) else return false;
  }

  //==============================================================================================
  // Actor Templates
  //==============================================================================================
  {
    ASSERT(actor_manager->create(fp)) else return false;
  }


  //==============================================================================================
  // Maps
  //==============================================================================================
  size_t number_of_maps = -1;
  fread(&number_of_maps, sizeof(number_of_maps), 1, fp);
  static const int MAX_NUMBER_OF_MAPS = 8;
  if (number_of_maps > MAX_NUMBER_OF_MAPS) {
    return false;
  }
  CONFIRM(world_manager->create(number_of_maps)) else return false;

  for (int i = 0; i < number_of_maps; ++i) {
    Map* map = world_manager->getMap(i);

    // Read the map header information
    CompiledAtlasServerHeader header;
    if (APP_ERROR(!fread(&header, sizeof(header), 1, fp))("Couldn't read server atlas header"))
      goto STARTUP_FAILED;

    if (i==0) { // todo: move outside of the map defn.
      mySpawnX = header.spawnX;
      mySpawnY = header.spawnY;
      mySpawnRadius = header.spawnRadius;
    }

    //myDefaultNavigability = header.defaultNavigability; // TODO

    CONFIRM(map->create(i, header.width, header.height)) else goto STARTUP_FAILED;

    //==============================================================================================
    // Navigability
    //==============================================================================================

    // Read the navigability map
    size_t navigabilityMapSize;
    MapNavigability* navigability;
    if (APP_ERROR(!fread(&navigabilityMapSize, sizeof(size_t), 1, fp) ||
              navigabilityMapSize != header.width * header.height)("Invalid navigability map") ||
      APP_FATAL(!(navigability = new MapNavigability[navigabilityMapSize]))("Server out of memory") ||
      APP_ERROR(!fread(navigability, sizeof(MapNavigability), navigabilityMapSize, fp))("Couldn't read navigability map")) {
        goto STARTUP_FAILED;
    }

    MapNavigability* location_navigability = navigability;
    MapLocationInfo* map_locations = map->getMapInfo();
    for (int y = 0; y < header.height; ++y) {
      for (int x = 0; x < header.width; ++x) {
        MapNavigability nav = *location_navigability;
        map_locations->navigability = nav;
        switch (nav) {
          case MAPNAV_WALK:
          case MAPNAV_WADE:
          case MAPNAV_SAFEZONE:
            map_locations->pass_by_moving = 1;
          case MAPNAV_LOWWALL:
            map_locations->pass_by_spirit = 1;
            break;

          default:
            map_locations->pass_by_moving = 0;
            map_locations->pass_by_spirit = 0;
            break;
        }

        // reset--filled in later
        map_locations->special_function_index = 0;

        location_navigability++;
        map_locations++;
      }
    }
    SAFE_DELETE_ARRAY(navigability);

    //==============================================================================================
    // Special Locations
    //==============================================================================================
    {
      size_t number;
      fread(&number, sizeof(number), 1, fp);
      CONFIRM(map->createSpecialFunctions(number)) else goto STARTUP_FAILED;
      for (size_t index = 0; index < number; ++index) {
        Evidyon::CompiledMapLocationSpecialFunction description;
        fread(&description, sizeof(description), 1, fp);
        MapLocationSpecialFunction* function =
          map->setLocationSpecialFunction(index, description.x, description.y);
        int map_index = description.map_link.map_index;
        function->map_link.target_map =
          (map_index < 0 || map_index >= number_of_maps) ? map : world_manager->getMap(map_index);
        function->map_link.x = description.map_link.x;
        function->map_link.y = description.map_link.y;
        function->type = description.type;
      }
    }

    //==============================================================================================
    // Swarm Spawn
    //==============================================================================================
    {
      int regions_wide, regions_high;
      fread(&regions_wide, sizeof(regions_wide), 1, fp);
      fread(&regions_high, sizeof(regions_high), 1, fp);

      for (int y = 0; y < regions_high; ++y) {
        for (int x = 0; x < regions_wide; ++x) {
          unsigned int treasure_randomizer_mask;
          fread(&treasure_randomizer_mask,
                sizeof(treasure_randomizer_mask), 1, fp);
          size_t local_swarms;
          fread(&local_swarms, sizeof(local_swarms),1, fp);
          Evidyon::CompiledRegionSwarmSpawn swarms[MAX_SWARM_SPAWNS_PER_WORLD_REGION];
          Evidyon::CompiledSwarmSpawnLocationInfo locations[MAX_SWARM_SPAWNS_PER_WORLD_REGION];
          ASSERT(local_swarms <= MAX_SWARM_SPAWNS_PER_WORLD_REGION) else return false;
          fread(swarms, sizeof(Evidyon::CompiledRegionSwarmSpawn), local_swarms, fp);
          fread(locations, sizeof(locations), 1, fp);

          // add to the map's spawner
          CONFIRM(map->createRegionSwarmSpawner(x,
                                                y,
                                                treasure_randomizer_mask,
                                                swarms,
                                                local_swarms,
                                                locations)) else return false;
        }
      }
    }

    //==============================================================================================
    // Inhabitants
    //==============================================================================================
    {
      size_t number_of_inhabitants = 0;
      fread(&number_of_inhabitants, sizeof(number_of_inhabitants), 1, fp);
      map->createInhabitants(number_of_inhabitants);
      for (size_t i = 0; i < number_of_inhabitants; ++i) {
        int x, y;
        size_t lifeform_ai_type;
        fread(&x, sizeof(x), 1, fp);
        fread(&y, sizeof(y), 1, fp);
        fread(&lifeform_ai_type, sizeof(lifeform_ai_type), 1, fp);
        map->setInhabitant(i, x, y, lifeform_ai_type);
      }
    }
  }



  // Close the game file
  fclose(fp);

  // Success

  singleton_ = this;
  return true;

STARTUP_FAILED:

  // Erase the file pointer
  if (fp) fclose(fp);

  // Free any allocated class-memory
  destroy();

  // Failure
  return false;
}

void GlobalGamefileManager::destroy() {
  singleton_ = NULL;
  SAFE_DELETE_ARRAY(myPlayerRaceArray);
  SAFE_DELETE_ARRAY(myPlayerClassArray);
  SAFE_DELETE_ARRAY(projectile_descriptions_);
  SAFE_DELETE_ARRAY(spell_descriptions_);
  SAFE_DELETE_ARRAY(magic_descriptions_);
}


//------------------------------------------------------------------------------------------------
size_t GlobalGamefileManager::getCharacterActorTypeIndex(int raceIndex, int genderIndex)
{
  if (APP_ERROR(raceIndex >= myPlayerRaceArraySize)("Invalid race index")) return 0xFFFFFFFF;
  unsigned int actor_description_template =
      genderIndex ? myPlayerRaceArray[raceIndex].female_actor_template_index :
                    myPlayerRaceArray[raceIndex].male_actor_template_index;
  GlobalActorManager* actor_manager = GlobalActorManager::singleton();
  return actor_manager->getDescriptionTemplate(actor_description_template)->actor_type;
}



//------------------------------------------------------------------------------------------------
const ServerItemDescription* GlobalGamefileManager::getItemDescription(size_t itemIndex)
{
  return (itemIndex < myItemDescriptionArraySize) ? &myItemDescriptionArray[itemIndex] : 0;
}


//------------------------------------------------------------------------------------------------
void GlobalGamefileManager::getPlayerRespawnLocation(float* x, float* y) const {
  *x = mySpawnX;
  *y = mySpawnY;
}


//------------------------------------------------------------------------------------------------
bool GlobalGamefileManager::isValidLevel1Character(
  int raceIndex, int classIndex, int attributes[5]) {
  // Make sure the indices are valid
  if (raceIndex  < 0 || raceIndex  >= myPlayerRaceArraySize ||
      classIndex < 0 || classIndex >= myPlayerClassArraySize) return false;

  // Compare all of the attributes
  int sum = 0;
  for (int a = 0; a < 5; ++a) {
  sum += attributes[a];
  if (attributes[a] < 10 || attributes[a] > myPlayerRaceArray[raceIndex].level1AttributeMaximums[a])
    return false;
  }
  CONFIRM(sum <= (10*5+25)) else return false;

  // This is a valid character
  return true;
}



//------------------------------------------------------------------------------------------------
int GlobalGamefileManager::getLevelUpExperience(int level) const {
  return (level > 0 && level <= HIGHEST_DEFINED_LEVEL) ? myLevelUpExperience[level-1] : INT_MAX;
}



//------------------------------------------------------------------------------------------------
int GlobalGamefileManager::getExperienceToAchieveLevel(int level) const {
  int experience = 0;
  --level;
  level = (level < HIGHEST_DEFINED_LEVEL) ? level : HIGHEST_DEFINED_LEVEL;
  for (int i = 0; i < level; ++i) {
    experience += myLevelUpExperience[i];
  }
  return experience;
}




//------------------------------------------------------------------------------------------------
bool GlobalGamefileManager::calculateAvatarAttributes(int class_index,
                                                      int level,
                                                      int stats[5],
                                                      int* hp,
                                                      int* mp) {
  // Make sure the class index is valid
  if (class_index < 0 || class_index >= myPlayerClassArraySize) return false;
  AvatarClassDescription* class_desc = &myPlayerClassArray[class_index];

  double calc_mp = class_desc->mpModifier * GameplayDesign::avatarCoreMP(level, stats[3], stats[4]);
  double calc_hp = class_desc->hpModifier * GameplayDesign::avatarCoreHP(level, stats[0], stats[2]);

  *mp = (int)calc_mp;
  *hp = (int)calc_hp;

  // Success
  return true;
}


//------------------------------------------------------------------------------------------------
bool GlobalGamefileManager::calculateCoreAvatarAttributes(AvatarAttributes* avatar_attributes) {

  // Make sure the class index is valid
  int class_index = avatar_attributes->classIndex;
  if (avatar_attributes == NULL || class_index < 0 || class_index >= myPlayerClassArraySize) return false;
  AvatarClassDescription* class_desc = &myPlayerClassArray[class_index];

  const int* stats = avatar_attributes->stats;

  const int level = avatar_attributes->base.level;
  double calc_mp = class_desc->mpModifier * GameplayDesign::avatarCoreMP(level, stats[3], stats[4]);
  double calc_hp = class_desc->hpModifier * GameplayDesign::avatarCoreHP(level, stats[0], stats[2]);

  avatar_attributes->base.max_hp = (int)calc_hp;
  avatar_attributes->base.max_mp = (int)calc_mp;

  avatar_attributes->base.speed = GameplayDesign::avatarMovementSpeed(stats[1]);
  avatar_attributes->base.attack_delay_time = GameplayDesign::avatarAttackDelay(stats[1]);
  avatar_attributes->base.hp_regen_rate = GameplayDesign::avatarHPRegenRate(calc_hp, stats[2]);
  avatar_attributes->base.mp_regen_rate = GameplayDesign::avatarMPRegenRate(calc_hp, stats[2]);
  avatar_attributes->base.min_attack_damage = GameplayDesign::avatarMeleeDamageBase(stats[0], level);
  avatar_attributes->base.random_attack_damage = GameplayDesign::avatarMeleeDamageBase(stats[0], level);

  { // Calculate the offensive factors
    avatar_attributes->offensive_magic_bonus = GameplayDesign::avatarOffensiveMagicBonus(level, stats[3], stats[4]);
    avatar_attributes->defensive_magic_bonus = GameplayDesign::avatarDefensiveMagicBonus(level, stats[4]);
  }

  avatar_attributes->base.attack_delay_time = GameplayDesign::avatarAttackDelay(stats[1]);
  avatar_attributes->base.casting_delay_percent = GameplayDesign::avatarCastingDelayPercent(stats[1], stats[3]);

  memcpy(&avatar_attributes->current,
         &avatar_attributes->base,
         sizeof(avatar_attributes->current));


  // Success
  return true;
}


//------------------------------------------------------------------------------------------------
float GlobalGamefileManager::getProjectileDuration(int projectile_index) const {
  return projectile_index < number_of_projectile_descriptions_ ?
  projectile_descriptions_[projectile_index].duration : 0.0f;
}


//------------------------------------------------------------------------------------------------
const SpellDescription* GlobalGamefileManager::getSpell(int spell_index) const {
  return (spell_index < number_of_spell_descriptions_) ?
    &spell_descriptions_[spell_index] : NULL;
}


//------------------------------------------------------------------------------------------------
const MagicDescription* GlobalGamefileManager::getMagic(int magic_index) const {
  return (magic_index < number_of_magic_descriptions_) ?
    &magic_descriptions_[magic_index] : NULL;
}


//------------------------------------------------------------------------------------------------
const SpellAvailabilityMask* GlobalGamefileManager::getClassSpellAvailability(int class_index, int level) {
  if (level > HIGHEST_DEFINED_LEVEL) level = HIGHEST_DEFINED_LEVEL;
  --level;  // level->index in array
  if (level < 0) level = 0;
  return (class_index < myPlayerClassArraySize) ?
  &myPlayerClassArray[class_index].spellAvailabilityMask[level] : NULL;
}

unsigned int GlobalGamefileManager::getHair(int race_index, int gender_index, int type_select) {
  if (race_index < 0 || race_index >= myPlayerRaceArraySize) return -1;
  if (gender_index < 0 || gender_index >= Evidyon::AVATARGENDER_COUNT) return -1;
  AvatarRaceDescription* race_desc = &myPlayerRaceArray[race_index];
  return race_desc->hair_styles[gender_index][type_select % Evidyon::AVATAR_RACE_HAIR_STYLES_PER_GENDER];  
}


unsigned int GlobalGamefileManager::getSpawnMagicFX() const {
  return spawn_magic_fx_;
}



}