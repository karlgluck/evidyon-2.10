#ifndef __SPECIALFXBINDING_ACTORENCHANTMENT_H__
#define __SPECIALFXBINDING_ACTORENCHANTMENT_H__

namespace Evidyon {

// This binding expires when an actor is removed or the enchantment
// slot that it monitors is vacated.
class SpecialFXBinding_ActorEnchantment : public SpecialFXBinding {
public:
  SpecialFXBinding_ActorEnchantment(Actor* actor, int enchantment);
  virtual void update(double time, double time_since_last_update);
  virtual void get(float* x, float* y, float* z);

private:
  Actor* actor_; // valid as long as the binding has not expired
  int enchantment_;
};


}


#endif