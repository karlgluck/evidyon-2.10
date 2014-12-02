#ifndef __ITEM_H__
#define __ITEM_H__

#include "itemidtype.h"

namespace Evidyon {

struct AvatarInventoryItem;
struct ServerItemDescription;


class Item {
public:
  void acquire(ItemIDType unique_id,
               int quantity,
               const ServerItemDescription* description);
  void release();

  inline ItemIDType getUniqueID() const { return unique_id_; }
  inline const ServerItemDescription* getDescription() const { return description_; }
  const unsigned int getGroundType() const;
  bool isStackable() const;
  inline int getQuantity() const { return quantity_; }
  void setQuantity(int quantity) { quantity_ = quantity; }
  inline unsigned int getSpecialPropertyFlags() const { return special_property_flags_; }

private:
  ItemIDType unique_id_;
  int quantity_;
  unsigned int special_property_flags_; // for now, always 0
  const ServerItemDescription* description_;
};

}

#endif