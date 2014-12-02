#include "avatarracelist.h"


AvatarRaceList::AvatarRaceList() {
  for (int i = 0; i < (int)Evidyon::AVATARRACE_COUNT; ++i) {
    member(Evidyon::AvatarRaceString((Evidyon::AvatarRace)i),
          &races_[i]);
  }
}

bool AvatarRaceList::compileForServer(dc::dcStreamOut* stream) const {
  { // todo: not needed because # of races is specified
    size_t classes = Evidyon::AVATARRACE_COUNT;
    stream->write(&classes, sizeof(classes));
  }
  for (int i = 0; i < (int)Evidyon::AVATARRACE_COUNT; ++i) {
    const PlayerCharacterRace* race = &races_[i];

    // Let the log know what we're doing
    DEBUG_INFO("Compiling race %s for server", race->getName().c_str());

    // Write each stat max
    for (int i = 0; i < 5; ++i) {
      unsigned char b;

      switch(i) {
        case 0: b = race->getMaxStrengthValue(); break;
        case 1: b = race->getMaxAgilityValue(); break;
        case 2: b = race->getMaxConstitutionValue(); break;
        case 3: b = race->getMaxIntelligenceValue(); break;
        case 4: b = race->getMaxWisdomValue(); break;
      }

      // Write this element
      if (!stream->write(&b, sizeof(unsigned char))) return false;
    }

    // The gender actor type templates
    for (int i = 0; i < (int)Evidyon::AVATARGENDER_COUNT; ++i) {
      int actor_type_template = race->getActorTypeTemplate((Evidyon::AvatarGender)i);
      if (!stream->write(&actor_type_template, sizeof(actor_type_template))) return false;
    }

    // Write the forms of hair
    size_t last_style = 0;
    for (int i = 0; i < (int)Evidyon::AVATARGENDER_COUNT; ++i) {
      for (int j = 0; j < (int)Evidyon::AVATAR_RACE_HAIR_STYLES_PER_GENDER; ++j) {
        size_t index = race->getHairStyle(i, j);
        if (index == (size_t)-1) index = last_style;
        stream->write(&index, sizeof(index));
        last_style = index;
      }
    }

  }

  return true;
}



bool AvatarRaceList::compileForClient(dc::dcStreamOut* stream) const {
  { // todo: not needed because races are specified
    size_t classes = Evidyon::AVATARRACE_COUNT;
    stream->write(&classes, sizeof(classes));
  }
  {
    for (int i = 0; i < (int)Evidyon::AVATARRACE_COUNT; ++i) {
      const PlayerCharacterRace* race = &races_[i];

      // Let the log know what we're doing
      DEBUG_INFO("Compiling race %s for client", race->getName().c_str());

      // Save the name of the race
      char race_name[CLIENTGAMEFILE_RACE_STRING_LENGTH];
      strcpy_s(race_name, CLIENTGAMEFILE_RACE_STRING_LENGTH, race->getName().c_str());
      stream->write(race_name, CLIENTGAMEFILE_RACE_STRING_LENGTH * sizeof(char));

      char race_desc[CLIENTGAMEFILE_RACE_DESCRIPTION_STRING_LENGTH];
      strcpy_s(race_desc, CLIENTGAMEFILE_RACE_DESCRIPTION_STRING_LENGTH, race->getDescription().c_str());
      stream->write(race_desc, CLIENTGAMEFILE_RACE_DESCRIPTION_STRING_LENGTH * sizeof(char));

      // Write each stat max
      for (int i = 0; i < 5; ++i)
      {
          unsigned char b;

          switch(i) 
          {
              case 0: b = race->getMaxStrengthValue(); break;
              case 1: b = race->getMaxAgilityValue(); break;
              case 2: b = race->getMaxConstitutionValue(); break;
              case 3: b = race->getMaxIntelligenceValue(); break;
              case 4: b = race->getMaxWisdomValue(); break;
          }

          // Write this element
          if (!stream->write(&b, sizeof(unsigned char))) return false;
      }
    }
  }

  return true;
}

std::string AvatarRaceList::staticTypeName() {
  return "AvatarRaceList";
}