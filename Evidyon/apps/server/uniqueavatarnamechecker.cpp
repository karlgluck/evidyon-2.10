#include "uniqueavatarnamechecker.h"
#include "../shared/evidyon_avatar.h"

namespace Evidyon {


void UniqueAvatarNameChecker::addName(const std::string& name) {
  std::string reduced_name;
  reduceName(name, &reduced_name);
  reduced_names_.insert(reduced_name);
}

bool UniqueAvatarNameChecker::isNameAvailable(const std::string& name) const {
  std::string reduced_name;
  reduceName(name, &reduced_name);
  return reduced_names_.find(reduced_name) == reduced_names_.end();
}

void UniqueAvatarNameChecker::reduceName(const std::string& name, std::string* reduced_name) {
  const char* source_name = name.c_str();
  int sentinel = 0;
  char current_character = '\0';
  while ((current_character = *(source_name++)) != '\0' &&
         (sentinel++) < Evidyon::MAX_AVATAR_NAME_LENGTH) {
    if ((current_character >= 'A' && current_character <= 'Z')) {
      current_character = current_character + 'a' - 'A'; // move to uppercase
    }
    if (current_character >= 'a' && current_character <= 'z') {
      reduced_name->append(1, current_character);
    }
  }
}


}