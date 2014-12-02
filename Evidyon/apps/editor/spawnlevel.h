#ifndef __SPAWNLEVEL_H__
#define __SPAWNLEVEL_H__

#include <dc/integer>

class SpawnLevel : public dc::dcResource<SpawnLevel> {
public:
  SpawnLevel();

  // Returns a random value between level_min_ and level_max_
  int getSpawnLevel() const;

public:
  static std::string staticTypeName();

private:
  dc::dcInteger level_min_, level_max_;
};



#endif