#include "item.h"
#include "../shared/evidyon_avatarinventory.h"
#include <dc/debug>
#include "../shared/server-editor/server_gamefile.h"

namespace Evidyon {


//------------------------------------------------------------------------------------------------
// Name: acquire
// Desc: When an item is generated and the item manager wants a container, this
//     method is invoked.
//------------------------------------------------------------------------------------------------
void Item::acquire(ItemIDType unique_id,
                   int quantity,
                   const ServerItemDescription* description) {
  unique_id_ = unique_id;
  quantity_ = quantity;
  special_property_flags_ = 0;
  description_ = description;
}

void Item::release() {
  unique_id_ = 0;
  description_ = NULL;
}

const unsigned int Item::getGroundType() const {
#if defined(EXTENDED_DEBUGGING)
  CONFIRM(description_);
#endif
  return description_->ground_type_index;
}

bool Item::isStackable() const {
#if defined(EXTENDED_DEBUGGING)
  CONFIRM(description_);
#endif
  return description_->max_stack_size > 1;
}


}