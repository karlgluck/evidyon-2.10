#ifndef __CLIENTENCHANTMENTDESCRIPTION_H__
#define __CLIENTENCHANTMENTDESCRIPTION_H__


namespace Evidyon {

struct ClientEnchantmentClassDescription {
  struct {
    int first_bit;
    int type_mask;
  } mask;
  int first_enchantment_index;
};

struct ClientEnchantmentDescription {
  int visual_fx;
  //int sound_fx;
};

}


#endif