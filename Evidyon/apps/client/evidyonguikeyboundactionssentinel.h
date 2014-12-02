#ifndef __EVIDYONGUIKEYBOUNDACTIONSSENTINEL_H__
#define __EVIDYONGUIKEYBOUNDACTIONSSENTINEL_H__

#include "../common/gui/guikeyboard.h"
#include "actiontargeter.h"

struct ClientItemDescription;

namespace dc {
class dcStreamOut;
class dcStreamIn;
}

namespace Evidyon {

class ActionTargeter;
struct ClientAvatarInventoryItem;
struct ClientSpellDescription;


class EvidyonGUIKeyBoundActionsSentinel : public GUIKeyboardSentinel {
public:
  EvidyonGUIKeyBoundActionsSentinel(ActionTargeter* targeter);

  void create(const ClientAvatarInventoryItem* inventory,
              const ClientItemDescription* item_descriptions,
              const ClientSpellDescription* spells);

  void setKey(GUIKey key, const Action* action);

  void setDefaultKeyBindings();
  void saveKeyBindings(dc::dcStreamOut* stream);
  bool readKeyBindings(dc::dcStreamIn* stream); // returns 'false' if the default keys were used

  // Finds the key that activates the given spell
  GUIKey getKeyForSpell(int spell_type);
  GUIKey getKeyForUserEvent(UserEventType type);
  GUIKey getKeyForConsumable(int item_type);

  // Finds the key that produces the given action
  GUIKey getKeyForAction(const Action* action);

  virtual bool onKeyDown(GUIKey key, GUIKeyboardInputSource* keyboard);
  virtual bool onKeyUp(GUIKey key, GUIKeyboardInputSource* keyboard);

private:
  ActionTargeter* targeter_;
  Action actions_[GUIKEY_COUNT];
  const ClientAvatarInventoryItem* inventory_;
  const ClientItemDescription* item_descriptions_;
  const ClientSpellDescription* spells_;
};


}



#endif

