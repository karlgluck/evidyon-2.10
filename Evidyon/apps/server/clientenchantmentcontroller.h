#ifndef __CLIENTENCHANTMENTCONTROLLER_H__
#define __CLIENTENCHANTMENTCONTROLLER_H__


#include "../shared/server-editor/magicdescription.h"
#include "avatarattributes.h"
#include "../shared/evidyon_enchantments.h"

namespace Evidyon {



struct Enchantment {
  double expiration_time;
  const MagicDescriptionStageEffect* effect;
  MagicDescriptionStageEffect::Parameters parameters;
};




class Client;


class ClientEnchantmentController {

public:
  ClientEnchantmentController();
  void reset(Client* owner);
  void clear();

  // Checks to see if any enchantments have expired.  If one does, this
  // method returns 'true' and the owner should call apply() with valid
  // parameters to sync the client's avatar's state.
  bool update(double time);

  // Sets a new enchantment.  This method will overwrite an enchantment
  // currently exists.  Calling this method will cause update() to
  // return 'true' until apply() is called.
  void addEnchantment(double expiration_time,
                      const MagicDescriptionStageEffect* effect,
                      const MagicDescriptionStageEffect::Parameters* parameters);

  // Applies active enchantments to the actor and fills out the bit mask
  // that can be sent to the client to provide the visualizations.
  bool apply(double time,
             AvatarAttributes::Enchantable* attributes,
             CorporealState* actor_state);

  // Gets the bit mask that describes what enchantments to display on the
  // actor visually.
  unsigned int getDescriptionBits() const;


private:
  Client* owner_;
  Enchantment enchantments_[Enchantments::NUMBER_OF_ENCHANTMENT_CLASSES];
  bool needs_updating_;
  double next_enchantment_expiration_time_;
  unsigned int description_bits_;
};

}


#endif