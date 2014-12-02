//------------------------------------------------------------------------------------------------
// File:    worldinteractionamanger.cpp
//
// Desc:    
//
// Copyright (c) 2007-2008 Unseen Studios.   All rights reserved.
//------------------------------------------------------------------------------------------------
#include "stdafx.h"
#include "worldinteractionmanager.h"

namespace WorldInput
{
static const float MOVE_ACTION_GENERATION_PERIOD = 1.0f / 3.0f;
}


//------------------------------------------------------------------------------------------------
// Name: WorldInteractionManager
// Desc: Initializes the class
//------------------------------------------------------------------------------------------------
WorldInteractionManager::WorldInteractionManager() : myMouseSentinel(this), myKeyboardSentinel(this)
{
    zero();
}



//------------------------------------------------------------------------------------------------
// Name: create
// Desc: Sets up this class for use
//------------------------------------------------------------------------------------------------
bool WorldInteractionManager::create(IsometricCamera* camera, GameFileData* gameFileData, VolucrisClient* client, LPDIRECT3DTEXTURE9 guiTexture)
{
    myCamera = camera;

    // TODO: use other parameters
    myMonitorInputFlag = true;
    myGameFileData = gameFileData;
    myClient = client;

    // Success
    return true;
}



//------------------------------------------------------------------------------------------------
// Name: destroy
// Desc: Frees this class's internal references
//------------------------------------------------------------------------------------------------
void WorldInteractionManager::destroy()
{
    unregisterInputSentinels();
    unregisterElements();
    zero();
}



//------------------------------------------------------------------------------------------------
// Name: registerElements
// Desc: Adds the GUI elements used by this class to the given layer
//------------------------------------------------------------------------------------------------
void WorldInteractionManager::registerElements(GUILayer* layer)
{
}



//------------------------------------------------------------------------------------------------
// Name: unregisterElements
// Desc: Removes the GUI elements from the layer
//------------------------------------------------------------------------------------------------
void WorldInteractionManager::unregisterElements()
{
}



//------------------------------------------------------------------------------------------------
// Name: update
// Desc: Called each frame to advance the input state
//------------------------------------------------------------------------------------------------
void WorldInteractionManager::update(double currentTime, double timeSinceLastUpdate)
{
  if (currentTime - myLastTimeMoveActionGenerated > WorldInput::MOVE_ACTION_GENERATION_PERIOD)
    {
        if (myCurrentMoveAction.type == ACTION_MOVE)
        {
            myCurrentMoveAction.move.defending = defending_;
            memcpy(createAction(ACTION_MOVE), &myCurrentMoveAction, sizeof(UserEvent));
            if (!myCurrentMoveAction.move.createdByDragging)
                ZeroMemory(&myCurrentMoveAction, sizeof(UserEvent));
            myLastTimeMoveActionGenerated = currentTime;
        }
    }

    // Use this effect
    if (myDevelopingEffectCombo.type == ACTION_USEEFFECTCOMBO && myDevelopingEffectKeysDown == 0 &&
        !(myDevelopingEffectNeedsTarget))
    {
        memcpy(createAction(ACTION_USEEFFECTCOMBO), &myDevelopingEffectCombo, sizeof(UserEvent));
        myDevelopingEffectCombo.type = ACTION_INVALID;
        myDevelopingEffectComboActive = false;
        myDevelopingEffectKeysDown = false;
    }
}



//------------------------------------------------------------------------------------------------
// Name: queryNextAction
// Desc: Obtains an action from the manager.  This method should be called until it
//       returns 'null' in order to flush the queue of actions developed each frame.
//------------------------------------------------------------------------------------------------
const WorldInteractionManager::UserEvent* WorldInteractionManager::queryNextAction()
{
    // Check to see if we're finished reading queued actions
    if (myNextActionToRead == myNextActionToWrite) return 0;

    // Get the next action and advance the read pointer
    UserEvent* actionToReturn = &myActions[myNextActionToRead];
    myNextActionToRead = (myNextActionToRead + 1) % ACTION_BUFFER_SIZE;

    // Return the action
    return actionToReturn;
}



//------------------------------------------------------------------------------------------------
// Name: registerInputSentinels
// Desc: Adds sentinels to the input sources to watch for user input
//------------------------------------------------------------------------------------------------
void WorldInteractionManager::registerInputSentinels(GUIMouseSentinel* parentMouseSentinel, GUIKeyboardSentinel* parentKeyboardSentinel)
{
    myMouseSentinel.setParentMouseSentinel(parentMouseSentinel);
    myKeyboardSentinel.setParentKeyboardSentinel(parentKeyboardSentinel);
}




//------------------------------------------------------------------------------------------------
// Name: unregisterInputSentinels
// Desc: Removes this manager's sentinels from the input chain
//------------------------------------------------------------------------------------------------
void WorldInteractionManager::unregisterInputSentinels()
{
    myMouseSentinel.removeFromMouseSentinelChain();
    myKeyboardSentinel.removeFromKeyboardSentinelChain();
}




//------------------------------------------------------------------------------------------------
// Name: monitorInput
// Desc: Sets the flag to determine whether or not this class should be watching
//       for user input.  Disabling this class is useful when changing screens,
//       for example.
//------------------------------------------------------------------------------------------------
void WorldInteractionManager::monitorInput(bool enable)
{
    myMonitorInputFlag = enable;
    myClient->myMouseInputSource.setMouseLock(!enable);

    // Reset the developing effect combo flags
    myDevelopingEffectCombo.type = WorldInteractionManager::ACTION_INVALID;
    myDevelopingEffectKeysDown = 0;
    myDevelopingEffectComboActive = false;
}





//------------------------------------------------------------------------------------------------
// Name: getKeyToEffectMapping
// Desc: Obtains the effect that is triggered by pressing each key
//------------------------------------------------------------------------------------------------
const unsigned int* WorldInteractionManager::getKeyToEffectMapping() const
{
    return myKeyToEffectIndexMap;
}



//------------------------------------------------------------------------------------------------
// Name: setKeyToEffectMapping
// Desc: Changes the effect that is triggered by pressing each key
//------------------------------------------------------------------------------------------------
void WorldInteractionManager::setKeyToEffectMapping(const unsigned int keyToEffectMapping[256])
{
    memcpy(myKeyToEffectIndexMap, keyToEffectMapping, sizeof(unsigned int) * 256);
}



//------------------------------------------------------------------------------------------------
// Name: createAction
// Desc: Returns an available structure for the given action type
//------------------------------------------------------------------------------------------------
WorldInteractionManager::UserEvent* WorldInteractionManager::createAction(ActionType actionType)
{
    UserEvent* actionToReturn = &myActions[myNextActionToWrite];
    myNextActionToWrite = (myNextActionToWrite + 1) % ACTION_BUFFER_SIZE;

    // Bump the read pointer forward if we are going to overwrite it
    if (myNextActionToRead == myNextActionToWrite)
        myNextActionToRead = (myNextActionToRead + 1) % ACTION_BUFFER_SIZE;

    // Set the type
    actionToReturn->type = actionType;

    // Return the available action structure
    return actionToReturn;
}



//------------------------------------------------------------------------------------------------
// Name: zero
// Desc: Resets this class
//------------------------------------------------------------------------------------------------
void WorldInteractionManager::zero()
{
    myMonitorInputFlag = false;
    myCamera = 0;
    ZeroMemory(myActions, sizeof(myActions));
    myNextActionToRead = myNextActionToWrite = 0;
    ZeroMemory(&myCurrentMoveAction, sizeof(myCurrentMoveAction));
    myLastTimeMoveActionGenerated = 0.0;
    myLayer = 0;
    memset(myKeyToEffectIndexMap, 0xFFFFFFFF, sizeof(myKeyToEffectIndexMap));
    myGameFileData = 0;

    ZeroMemory(&myDevelopingEffectCombo, sizeof(myDevelopingEffectCombo));
    myDevelopingEffectKeysDown = 0;
    myDevelopingEffectNeedsTarget = false;
    myDevelopingEffectComboActive = false;
}



//------------------------------------------------------------------------------------------------
// Name: KeyboardSentinel
// Desc: Initializes this class
//------------------------------------------------------------------------------------------------
WorldInteractionManager::KeyboardSentinel::KeyboardSentinel(WorldInteractionManager* owner)
{
    myOwner = owner;
}



//------------------------------------------------------------------------------------------------
// Name: onKeyDown
// Desc: When a key is pressed, this method is invoked by the keyboard input source
//       @return 'true' To indicate the event should stop propogating, 'false' otherwise
//------------------------------------------------------------------------------------------------
bool WorldInteractionManager::KeyboardSentinel::onKeyDown(GUIKey key, GUIKeyboardInputSource* keyboard)
{
    if (!myOwner->myMonitorInputFlag) return false;

    // Add to or clear the ability combo
    unsigned int effectIndex = myOwner->myKeyToEffectIndexMap[key];
    if (effectIndex > MAX_SPELL_TYPES)
    {
        if (myOwner->myDevelopingEffectComboActive) {
            myOwner->myDevelopingEffectCombo.type = WorldInteractionManager::ACTION_INVALID;
            myOwner->myDevelopingEffectKeysDown = 0;
            myOwner->myDevelopingEffectComboActive = false;
            myOwner->createAction(ACTION_ENDEFFECTCOMBO);
        }
    }
    else
    {
        myOwner->myDevelopingEffectCombo.type = WorldInteractionManager::ACTION_USEEFFECTCOMBO;
        int keysDown = myOwner->myDevelopingEffectKeysDown;
        if (keysDown < 3) {
          if (keysDown == 0) {
            myOwner->myDevelopingEffectNeedsTarget =
              myOwner->myGameFileData->spells.spell_descriptions_[effectIndex].needs_target;
          }
          myOwner->myDevelopingEffectCombo.useEffectCombo.effects[keysDown] = effectIndex;
        }

        ++keysDown;

        // Invalidate effects after this one
        for (int i = keysDown; i < 3; ++i) {
            myOwner->myDevelopingEffectCombo.useEffectCombo.effects[i] = 0xFFFFFFF;
        }

        myOwner->myDevelopingEffectKeysDown = keysDown;
        if (!myOwner->myDevelopingEffectComboActive) {
            myOwner->myDevelopingEffectComboActive = true;
            myOwner->createAction(ACTION_STARTEFFECTCOMBO);
        }
    }

    return false;
}



//------------------------------------------------------------------------------------------------
// Name: onKeyUp
// Desc: When a key is released, this method is invoked by the keyboard input source
//       @return 'true' To indicate the event should stop propogating, 'false' otherwise
//------------------------------------------------------------------------------------------------
bool WorldInteractionManager::KeyboardSentinel::onKeyUp(GUIKey key, GUIKeyboardInputSource* keyboard)
{
    if (!myOwner->myMonitorInputFlag) return false;

    if (myOwner->myDevelopingEffectKeysDown)
        --myOwner->myDevelopingEffectKeysDown;

    return false;
}



//------------------------------------------------------------------------------------------------
// Name: onTypedCharacter
// Desc: Called when the user types a key associated with a printable character
//       @return 'true' To indicate the event should stop propogating, 'false' otherwise
//------------------------------------------------------------------------------------------------
bool WorldInteractionManager::KeyboardSentinel::onTypedCharacter(char character, GUIKeyboardInputSource* keyboard)
{
    if (!myOwner->myMonitorInputFlag) return false;
    return false;
}


//------------------------------------------------------------------------------------------------
// Name: MouseSentinel
// Desc: Initializes this class
//------------------------------------------------------------------------------------------------
WorldInteractionManager::MouseSentinel::MouseSentinel(WorldInteractionManager* owner)
{
    myOwner = owner;
    myLastMousePosition.zero();
}



//------------------------------------------------------------------------------------------------
// Name: onMouseMove
// Desc: Called by the mouse input source when the mouse moves.  The default implementation propogates
//       the message based on the focus/hierarchy structure established for the sentinels.
//       @return 'true' To indicate the event should stop propogating
//------------------------------------------------------------------------------------------------
bool WorldInteractionManager::MouseSentinel::onMouseMove(GUIPoint position, GUIPoint oldPosition, GUIMouseInputSource* mouse)
{
    myLastMousePosition = position;
    if (!myOwner->myMonitorInputFlag) return false;
    if (myOwner->myDevelopingEffectComboActive) return false;
    if (mouse->isPrimaryButtonDown()) {
      myOwner->myCurrentMoveAction.type = ACTION_MOVE;
      myOwner->myCurrentMoveAction.move.createdByDragging = true;
      myOwner->myCurrentMoveAction.move.screenPoint = position;
    }

    return false;
}



//------------------------------------------------------------------------------------------------
// Name: onMouseButtonAction
// Desc: Called by the mouse input source when a mouse button alters its state.  The default implementation propogates
//       the message based on the focus/hierarchy structure established for the sentinels.
//       @return 'true' To indicate the event should stop propogating
//------------------------------------------------------------------------------------------------
bool WorldInteractionManager::MouseSentinel::onMouseButtonAction(GUIMouseButtonAction action, GUIPoint position, GUIMouseInputSource* mouse)
{
    myLastMousePosition = position;
    if (!myOwner->myMonitorInputFlag) return false;

    if (!myOwner->myDevelopingEffectComboActive) {
      if (action == GUIMOUSE_DOWN) {
          myOwner->myCurrentMoveAction.type = ACTION_MOVE;
          myOwner->myCurrentMoveAction.move.createdByDragging = false;
          myOwner->myCurrentMoveAction.move.screenPoint = position;
          //DEBUG_INFO("GENERATED BY onMouseButtonAction");
      } else if (action == GUIMOUSE_UP && myOwner->myCurrentMoveAction.move.createdByDragging)
      {
        myOwner->myCurrentMoveAction.type = ACTION_MOVE;
        myOwner->myCurrentMoveAction.move.createdByDragging = false;
        myOwner->myCurrentMoveAction.move.screenPoint = position;
        //DEBUG_INFO("GENERATED BY onMouseButtonAction - end dragging");
      }
    } else {
      if (action == GUIMOUSE_CLICK_SINGLE) {
        if (myOwner->myDevelopingEffectNeedsTarget) {
          float x, y;
          myOwner->myCamera->getWorldLocation(position.x, position.y, &x, &y);
          myOwner->myDevelopingEffectCombo.useEffectCombo.target_x = x;
          myOwner->myDevelopingEffectCombo.useEffectCombo.target_y = y;

          // Always find a target actor
          myOwner->myClient->findActorNearLocation(x, y,
                                                  &myOwner->myDevelopingEffectCombo.useEffectCombo.target_actor);
          myOwner->myDevelopingEffectNeedsTarget = false;
        }
      }
      else if (action == GUIMOUSE_CLICK_ALT_SINGLE) {
        if (myOwner->myDevelopingEffectComboActive) {  // cancel developing effect
          myOwner->myDevelopingEffectCombo.type = WorldInteractionManager::ACTION_INVALID;
          myOwner->myDevelopingEffectKeysDown = 0;
          myOwner->myDevelopingEffectComboActive = false;
          myOwner->createAction(ACTION_ENDEFFECTCOMBO);
        }
      }
    }

    return false;
}


//------------------------------------------------------------------------------------------------
// Name: getLastMousePosition
// Desc: Obtains the most recently updated coordinates of the mouse
//------------------------------------------------------------------------------------------------
GUIPoint WorldInteractionManager::MouseSentinel::getLastMousePosition() const
{
    if (myOwner->myLayer)
        return myOwner->myLayer->getPrimaryCanvas()->getArea()->constrain(myLastMousePosition);
    else
        return myLastMousePosition;
}
