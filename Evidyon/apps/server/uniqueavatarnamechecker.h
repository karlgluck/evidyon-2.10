#ifndef __UNIQUEAVATARNAMECHECKER_H__
#define __UNIQUEAVATARNAMECHECKER_H__


#include <string>
#include <set>


namespace Evidyon {

class UniqueAvatarNameChecker {
public:
  void addName(const std::string& name);
  bool isNameAvailable(const std::string& name) const;

public:

  // translates a name into a standardized form that can be
  // compared with other names for uniqueness
  static void reduceName(const std::string& name, std::string* reduced_name);

private:
  std::set<std::string> reduced_names_;
};

}

#endif