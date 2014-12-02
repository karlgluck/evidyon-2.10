#include "spawnlevel.h"



SpawnLevel::SpawnLevel() {
  member("Min Level", &level_min_);
  member("Max Level", &level_max_);
}

int SpawnLevel::getSpawnLevel() const {
  int min_level = level_min_.getValue();
  int range = level_max_.getValue() - min_level;
  return range == 0 ? min_level : (rand()%range + min_level);
}

std::string SpawnLevel::staticTypeName() {
  return "Spawn Level";
}
