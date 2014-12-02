//------------------------------------------------------------------------------------------------
// File:  worldinteractionamanger.h
//
// Desc:  
//
// Copyright (c) 2007-2008 Unseen Studios.   All rights reserved.
//------------------------------------------------------------------------------------------------
#ifndef __WORLDINTERACTIONMANAGER_H__
#define __WORLDINTERACTIONMANAGER_H__


#include "../shared/actorid.h"

/// The maximum number of actions to store in a single frame
#define ACTION_BUFFER_SIZE  30

/**
 * This class is responsible for handling how the user provides input to the world when not
 * inside of a menu or in another format
 */
class WorldInteractionManager
{
  class MouseSentinel : public GUIMouseSentinel
  {
    public:

      /**
       * Initializes this class
       */
      MouseSentinel(WorldInteractionManager* owner);

    public:

      /**
       * Called by the mouse input source when the mouse moves.  The default implementation propogates
       * the message based on the focus/hierarchy structure established for the sentinels.
       * @return 'true' To indicate the event should stop propogating
       */
      bool onMouseMove(GUIPoint position, GUIPoint oldPosition, GUIMouseInputSource* mouse);

      /**
       * Called by the mouse input source when a mouse button alters its state.  The default implementation propogates
       * the message based on the focus/hierarchy structure established for the sentinels.
       * @return 'true' To indicate the event should stop propogating
       */
      bool onMouseButtonAction(GUIMouseButtonAction action, GUIPoint position, GUIMouseInputSource* mouse);

      /** 
       * Obtains the most recently updated coordinates of the mouse
       */
      GUIPoint getLastMousePosition() const;


    protected:

      /// The class that contains this one
      WorldInteractionManager* myOwner;

      /// The last known coordinates of the mouse
      GUIPoint myLastMousePosition;
  };

  class KeyboardSentinel : public GUIKeyboardSentinel
  {
    public:

      /**
       * Initializes this class
       */
      KeyboardSentinel(WorldInteractionManager* owner);


    public:

      /**
       * When a key is pressed, this method is invoked by the keyboard input source
       * @return 'true' To indicate the event should stop propogating, 'false' otherwise
       */
      bool onKeyDown(GUIKey key, GUIKeyboardInputSource* keyboard);

      /**
       * When a key is released, this method is invoked by the keyboard input source
       * @return 'true' To indicate the event should stop propogating, 'false' otherwise
       */
      bool onKeyUp(GUIKey key, GUIKeyboardInputSource* keyboard);

      /**
       * Called when the user types a key associated with a printable character
       * @return 'true' To indicate the event should stop propogating, 'false' otherwise
       */
      bool onTypedCharacter(char character, GUIKeyboardInputSource* keyboard);


    protected:

      /// The class that contains this one
      WorldInteractionManager* myOwner;
  };

  public:

    /**
     * Different possible action types
     */
    enum ActionType
    {
      ACTION_INVALID,
      ACTION_MOVE,
      ACTION_CHANGEWORLDVIEW,
      ACTION_STARTEFFECTCOMBO,
      ACTION_ENDEFFECTCOMBO,
      ACTION_USEEFFECTCOMBO,
    };

    /**
     * Stores an action that the user wants to perform
     */
    struct UserEvent
    {
      ActionType type;

      union
      {
        struct
        {
          bool createdByDragging;
          GUIPoint screenPoint;
          bool defending;
        } move;

        struct
        {
          bool stop_action;
          bool now_blocking;
          GUIPoint screenPoint;
        } stopBlock;

        struct
        {
          bool tilted;
        } changeWorldView;

        struct
        {
          float target_x, target_y;
          ActorID target_actor;
          unsigned int effects[3];
        } useEffectCombo;
      };
    };

  public:

    /**
     * Initializes the class
     */
    WorldInteractionManager();

    /**
     * Sets up this class for use
     */
    bool create(IsometricCamera* camera, GameFileData* gameFileData, VolucrisClient* client, LPDIRECT3DTEXTURE9 guiTexture);

    /**
     * Frees this class's internal references
     */
    void destroy();

    /**
     * Adds the GUI elements used by this class to the given layer
     */
    void registerElements(GUILayer* layer);

    /**
     * Removes the GUI elements from the layer
     */
    void unregisterElements();

    /**
     * Called each frame to advance the input state
     */
    void update(double currentTime, double timeSinceLastUpdate);

    /**
     * Obtains an action from the manager.  This method should be called until it
     * returns 'null' in order to flush the queue of actions developed each frame.
     */
    const UserEvent* queryNextAction();

    /**
     * Adds sentinels to the input sources to watch for user input
     */
    void registerInputSentinels(GUIMouseSentinel* parentMouseSentinel, GUIKeyboardSentinel* parentKeyboardSentinel);

    /**
     * Removes this manager's sentinels from the input chain
     */
    void unregisterInputSentinels();

    /**
     * Sets the flag to determine whether or not this class should be watching
     * for user input.  Disabling this class is useful when changing screens,
     * for example.
     */
    void monitorInput(bool enable);

    /**
     * Obtains the effect that is triggered by pressing each key
     */
    const unsigned int* getKeyToEffectMapping() const;

    /**
     * Changes the effect that is triggered by pressing each key
     */
    void setKeyToEffectMapping(const unsigned int keyToEffectMapping[256]);


  protected:

    /**
     * Returns an available structure for the given action type
     */
    UserEvent* createAction(ActionType actionType);

    /**
     * Resets this class
     */
    void zero();


  protected:

    /// Processes mouse input
    MouseSentinel myMouseSentinel;

    /// Processes keyboard input
    KeyboardSentinel myKeyboardSentinel;

    /// Whether or not to watch for user input and develop actions
    bool myMonitorInputFlag;

    /// The camera to use to transform screen-coordinates into world-coordinates
    IsometricCamera* myCamera;

    /// A ring-buffer of actions
    UserEvent myActions[ACTION_BUFFER_SIZE];
    int myNextActionToRead, myNextActionToWrite;

    /// The current move action is copied into the ring buffer when the motion
    /// frequency timer expires.  Until that point, it is continuously
    /// overwritten.
    UserEvent myCurrentMoveAction;
    bool defending_;
    double myLastTimeMoveActionGenerated;

    /// The layer in which this class's elements are stored
    GUILayer* myLayer;

    /// Mappings of keys -> effect types.  Unmapped keys are set to 0xFFFFFFFF.
    unsigned int myKeyToEffectIndexMap[256];

    /// The developing effect combo
    UserEvent myDevelopingEffectCombo;
    int myDevelopingEffectKeysDown;
    bool myDevelopingEffectNeedsTarget;
    bool myDevelopingEffectComboActive;

    /// Used for settings
    GameFileData* myGameFileData;

    /// Used to find actor targets when targeting effects
    VolucrisClient* myClient;
};














#endif
