#ifndef __GLOBALGAMEFILEMANAGER_H__
#define __GLOBALGAMEFILEMANAGER_H__

#include "../shared/evidyon_actor.h"
#include "../shared/evidyon_avatar.h"

namespace Evidyon {

struct ServerEffectDescription;
struct ServerItemDescription;
struct AvatarRaceDescription;
struct AvatarClassDescription;
enum MapNavigability;
struct MagicDescription;
class GlobalWorldManager;
class GlobalLifeformAIManager;
class GlobalActorManager;
class GlobalItemManager;
class UniqueAvatarNameChecker;

struct SpellDescription {
  const MagicDescription* magic;
  int mp_cost;
};

struct ServerProjectileDescription;
struct AvatarAttributes;
class Actor;
class UniqueAvatarNameChecker;


class GlobalGamefileManager {
public:
  static GlobalGamefileManager* singleton();


public:
  bool create(const char* source_file,
              UniqueAvatarNameChecker* unique_avatar_name_checker,
              GlobalItemManager* item_manager,
              GlobalActorManager* actor_manager,
              GlobalWorldManager* world_manager,
              GlobalLifeformAIManager* lifeform_ai_manager);
  void destroy();

  ActorTypeIndex getCharacterActorTypeIndex(int raceIndex, int genderIndex);
  const ServerItemDescription* getItemDescription(size_t itemIndex);
  void getPlayerRespawnLocation(float* x, float* y) const;


  // Determines whether or not the character specified by the given values is
   // valid for a newly-created level 1 character
  bool isValidLevel1Character(int raceIndex, int classIndex, int attributes[5]); // TODO: int[5] should be a type...or something

  int getLevelUpExperience(int level) const;
  int getExperienceToAchieveLevel(int level) const;

  bool calculateAvatarAttributes(int class_index,
                                 int level,
                                 int stats[5],
                                 int* hp,
                                 int* mp);
  bool calculateCoreAvatarAttributes(AvatarAttributes* attributes);

  float getProjectileDuration(int projectile_index) const;
  const SpellDescription* getSpell(int spell_index) const;
  const MagicDescription* getMagic(int magic_index) const;
  const SpellAvailabilityMask* getClassSpellAvailability(int class_index, int level);

  unsigned int getHair(int race_index, int gender_index, int type_select);

  unsigned int getSpawnMagicFX() const;

private:
  /// The size of the map
  size_t myMapWidth, myMapHeight;

  /// Where to spawn players
  int mySpawnX, mySpawnY;
  float mySpawnRadius;

  /// Amount of experience required to gain each level
  int myLevelUpExperience[HIGHEST_DEFINED_LEVEL];

  /// A list of races and their attributes
  AvatarRaceDescription* myPlayerRaceArray;
  size_t myPlayerRaceArraySize;

  /// A list of classes and their attributes
  AvatarClassDescription* myPlayerClassArray;
  size_t myPlayerClassArraySize;

  ServerProjectileDescription* projectile_descriptions_;
  size_t number_of_projectile_descriptions_;

  SpellDescription* spell_descriptions_;
  size_t number_of_spell_descriptions_;

  MagicDescription* magic_descriptions_;
  size_t number_of_magic_descriptions_;

  /// The items in the game
  ServerItemDescription* myItemDescriptionArray;
  size_t myItemDescriptionArraySize;

  unsigned int spawn_magic_fx_;

private:
  static GlobalGamefileManager* singleton_;
};

}

#endif