#ifndef __CLIENTSPELLDESCRIPTION_H__
#define __CLIENTSPELLDESCRIPTION_H__


namespace Evidyon {


struct ClientSpellDescription {
static const int MAX_NAME_LENGTH = 32;
static const int MAX_DESCRIPTION_LENGTH = 128;

  char name[MAX_NAME_LENGTH];
  char description[MAX_DESCRIPTION_LENGTH];
  int mp_cost;

  bool needs_target;
};

}


#endif