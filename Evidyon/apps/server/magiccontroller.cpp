#include "magiccontroller.h"
#include "magicgenerator.h"
#include "../common/null.h"

namespace Evidyon {

MagicController::MagicController() {
  reset(NULL);
}

void MagicController::reset(MagicGenerator* generator)  {
  generator_ = generator;
  stop();
}

bool MagicController::update(double time) {
  bool check_state = false;
  bool state_changed = false;
  do {
    check_state = false;
    switch (state_) {
      case IDLE:{
        if (cast_spell_) {
          state_ = CAST_SPELL;
          check_state = true;
        }
      } break;

      case CAST_SPELL:{
        if (cast_spell_) {
          cast_spell_ = false;
          bool freeze = false;
          if (generator_->cast(time,
                               spell_source_,
                              &spell_to_cast_,
                              &freeze,
                              &motion_timeout_,
                              &repeat_timeout_)) {
            if (motion_timeout_ < repeat_timeout_) {
              state_ = freeze ? DELAY_FREEZE_REPEAT : DELAY_ROOT_REPEAT;
            } else {
              state_ = freeze ? DELAY_REPEAT_FREEZE : DELAY_REPEAT_ROOT;
            }
            check_state = true;
          } else {  // spell couldn't be cast
            state_ = IDLE;
          }

          spell_source_ = 0;
          spell_to_cast_.description = NULL;
        } else {  // spell was not activate so we're mistakenly in this state
          state_ = IDLE;
        }
      } break;

      case DELAY_REPEAT: {
        if (time >= repeat_timeout_) {
          state_ = IDLE;
          check_state = true;
        }
      } break;

      case DELAY_REPEAT_FREEZE:{
        if (time >= repeat_timeout_) {
          state_ = DELAY_FREEZE;
          check_state = true;
        }
      } break;

      case DELAY_REPEAT_ROOT:{
        if (time >= repeat_timeout_) {
          state_ = DELAY_ROOT;
          check_state = true;
        }
      } break;

      case DELAY_ROOT_REPEAT:
      case DELAY_FREEZE_REPEAT:
        if (time >= motion_timeout_) {
          state_ = DELAY_REPEAT;
          check_state = true;
        }
      break;

      case DELAY_FREEZE:
      case DELAY_ROOT:
        if (time >= motion_timeout_) {
          state_ = IDLE;
          check_state = true;
        }
        if (cast_spell_) {
          state_ = CAST_SPELL;
          check_state = true;
        }
      break;
    }
    if (check_state) {
      state_changed = true;
    }
  } while(check_state);
  return state_changed;
}

bool MagicController::busy() const {
  return state_ != IDLE;
}

void MagicController::cast(int source, const Magic* magic) {
  if (magic == NULL) {
    cast_spell_ = false;
    spell_source_ = 0;
  } else {
    cast_spell_ = true;
    spell_to_cast_ = *magic;
    spell_source_ = source;
  }
}

double MagicController::timeToCast(double time) const {
  switch (state_) {
    case IDLE:
    case CAST_SPELL: return 0.0;
    default:     return repeat_timeout_ - time;
  }
}


bool MagicController::rooted() const {
  // todo: optimize this--it should only take a bitmask and a comparison
  return frozen() || state_ == DELAY_ROOT || state_ == DELAY_REPEAT_ROOT || state_ == DELAY_ROOT_REPEAT;
}

bool MagicController::frozen() const {
  // todo: optimize this--it should only take a bitmask and a comparison
  return state_ == DELAY_FREEZE || state_ == DELAY_REPEAT_FREEZE || state_ == DELAY_FREEZE_REPEAT;
}

void MagicController::stop() {
  state_ = IDLE;
  motion_timeout_ = 0.0;
  repeat_timeout_ = 0.0;
  cast_spell_ = false;
}

void MagicController::freeze(double time_to_unfreeze) {
  state_ = DELAY_FREEZE_REPEAT;
  motion_timeout_ = time_to_unfreeze;
  repeat_timeout_ = time_to_unfreeze;
  cast_spell_ = false;
}


}