#ifndef __CLIENTPROJECTILE_H__
#define __CLIENTPROJECTILE_H__


namespace Evidyon {

struct ClientProjectileDescription {
  float duration;
  int carrier_fx, termination_fx;
  int carrier_sound, termination_sound;
};

}


#endif