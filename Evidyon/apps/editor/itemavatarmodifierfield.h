#ifndef __ITEMAVATARMODIFIERFIELD_H__
#define __ITEMAVATARMODIFIERFIELD_H__

#include <dc/enum>

enum ItemAvatarModifierFieldState {
  FIELDSTATE_DISABLED,
  FIELDSTATE_INNATE_ATTRIBUTE,
  FIELDSTATE_PROPPED_ATTRIBUTE,
};


template <typename ValueType> class ItemAvatarModifierField : public dc::dcResource<ItemAvatarModifierField<ValueType>> {
public:
  ItemAvatarModifierField() {
    member("State", &state_);
    member("Value", &value_);
  }

  void setFieldStateValue(ItemAvatarModifierFieldState state) { state_.setValue(state); }
  ItemAvatarModifierFieldState getFieldStateValue() const { return state_.getValue(); }
  const ValueType& getValue() const { return value_; }
  ValueType getValue() { return value_; }

public:
  static std::string staticTypeName() {
    return std::string("ItemAvatarModifierField") + ValueType::staticTypeName();
  }

private:
  dc::dcEnum<ItemAvatarModifierFieldState> state_;
  ValueType value_;
};



#endif