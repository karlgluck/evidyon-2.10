//------------------------------------------------------------------------------------------------
// File:  volucrisclient.h
//
// Desc:  The Project Volucris client state machine
//
// Copyright (c) 2007-2008 Unseen Studios.   All rights reserved.
//------------------------------------------------------------------------------------------------
#ifndef __VOLUCRISCLIENT_H__
#define __VOLUCRISCLIENT_H__

#include <hash_map>
#include "actortypedescription.h"
#include "globalactormanager.h"
#include "globalrenderingmanager.h"
#include "globalmapmanager.h"
#include "globalspecialfxmanager.h"
#include "globalsoundmanager.h"


#define NUMBER_OF_SOUND_BUFFERS 5

#define SPECIALFX_MAX_POINTPARTICLES  4000


#include "actor.h"



#define CLIENT_MAP_CACHE_WIDTH_IN_CHUNKS  8
#define CLIENT_MAP_CACHE_HEIGHT_IN_CHUNKS   6










/**
 * Contains all of the programming for the Volucris client application
 */
class VolucrisClient {
  /**
   * This declaration holds the different states that the application can be in.
   */
  enum State
  {
    VCS_STARTUP,
    VCS_CONNECT,
    VCS_ACCOUNT_LOGIN,
    VCS_CREATE_ACCOUNT,
    VCS_LOGGED_INTO_ACCOUNT,
    VCS_CREATE_CHARACTER,
    VCS_WORLD_MAIN,
    VCS_DISCONNECT,
    VCS_NO_CONNECTION,
    VCS_SHUTDOWN,

    VCS_LOST_D3DDEVICE,

    VCS_DEBUGSTATESANDBOX,
  };

  /**
   * Gets the name of the state address provided.  This is used for debugging.
   */
  const char* stateName(State state);

  /**
   * Runs the state with the provided address, and returns the next state to execute
   */
  void run(State state);


public:

  /**
   * Initializes the class
   */
  VolucrisClient();

  /**
   * Runs the client application
   */
  void execute();

  /**
   * Handles a Windows message
   */
  void handleWindowsMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);


protected:

  /**
   * Shuts down all of the program components and erases allocated memory when the program
   * is about to close.
   */
  void cleanup();


// Program methods
protected:

  /**
   * Executes a robust algorithm to create the Direct3D device.  Implemented in
   * the source file acquiregraphics.cpp
   */
  bool createD3DDevice(bool windowed, unsigned int requiredNumberOfBlendMatrices);

  /**
   * Executes windows messages and returns when the next frame is to be run.  This
   * method will return false if the application should exit.
   */
  bool windowsMessagePump();

  /**
   * Initializes the system components
   */
  bool acquireSystem();

  /**
   * Initializes network components
   */
  bool acquireNetwork();

  /**
   * Initializes graphics components
   */
  bool acquireGraphics();

  /**
   * Initializes input components
   */
  bool acquireInput();
  
  /**
   * Initializes sound components
   */
  bool acquireAudio();

  /**
   * Loads the game file into the resources structure
   */
  bool acquireResources();

  /**
   * Unloads everything in the resources structure
   */
  void freeResources();


// Program states
protected:

  void stateStartup();
  void stateConnect();
  void stateAccountLogin();
  void stateCreateAccount();
  void stateLoggedIntoAccount();
  void stateCreateCharacter();
  void stateWorldMain();
  void stateDisconnect();
  void stateNoConnection();
  void stateShutdown();
  void stateLostD3DDevice();

// Debugging states
  void debugStateSandbox();


protected:

  /// This state machine object is used to control transitions between execution methods
  dcxStateMachine<State> myStateMachine;

  /// Client settings loaded from the hard drive
  dcxIniFileReader myConfig;
public:
  /// Mouse-input controller
  VolucrisGUIMouseInputSource myMouseInputSource;
private:
  /// Keyboard-input controller
  VolucrisGUIKeyboardInputSource myKeyboardInputSource;

  // Components of the graphics solution
  HWND myMainWindow;
  D3DPRESENT_PARAMETERS myD3DParams;
  bool myUsingMixedDeviceFlag;
  bool myUsingSoftwareDeviceFlag;
  LPDIRECT3D9 myDirect3D;
  LPDIRECT3DDEVICE9 myD3DDevice;
  double myTargetFPS;
  double myNextRenderingTime;   // The next time at which to render a frame; limits the client's frame rate
  double myConfiguredFramePeriod; // Config-file based delay time for rendering

  /// Network components
  ENetHost* myENetClient;
  ENetPeer* myENetServer;

  /// The source file for reading more gameplay data
  FILE* myResourceFile;

  /// All of the gameplay data loaded from the source file
  GameFileData myResources;

  /// Various stuff used in the GUI
  GUIFont myWindowFont;
  GUICanvas mouse_position_canvas_;
  GUICanvas mouse_cursor_canvas_;
  GUIPictureElement mouse_cursor_;
  GUILayerBasic gui_primary_layer_;


  void renderLoginBackground();
  void updateMouseRenderingLocation();  // called automatically by allowFrameToBeRendered
  void setMouseCursorNormal();
  void setMouseCursorActive();


public:
  void getMouseWorldLocation(float* x, float* z) { GUIPoint p = myMouseInputSource.getMousePosition(); myIsometricCamera.getWorldLocation(p.x, p.y, x, z); }
  void findActorNearLocation(float x, float z, ActorID* id) { global_actor_manager_.findActorNearLocation(x, z, 0, id, true); }


protected:

  /**
   * If this method returns 'true', the calling method should render a frame.  This
   * limits the rate at which frames are drawn to the screen
   */
  bool allowFrameToBeRendered(double frameTime);

  /// The camera used for rendering the world
  IsometricCamera myIsometricCamera;


private:
  GlobalMapManager global_map_manager_;


/// Manage actors in the world
protected:
  GlobalActorManager global_actor_manager_;
  Actor* getActor(int id) { return global_actor_manager_.get(id); }

  /// Information about this player
  struct
  {
    bool male;
    int raceIndex;
    int classIndex;
    float currentHPPercent, currentMPPercent, currentEXPPercent;
    int attributes[5];
    ClientAvatarInventoryItem inventory[AVATAR_INVENTORY_SIZE];
    int inventory_update_indices_[AVATAR_INVENTORY_SIZE];
    int highest_inventory_update_index_;
    int currentHP, maxHP;
    int currentMP, maxMP;
    SpellAvailabilityMask effectAvailability;
    char name[MAX_AVATAR_NAME_LENGTH];
    unsigned int money, geonite;
  } myCharacter;

  double time_of_day_adjustment_;

protected:
  SceneryRenderer mySceneryRenderer;

  typedef std::multimap<std::pair<int,int>,HSCENERY> ItemsOnMapTable;
  ItemsOnMapTable myItemsOnMapTable;

  GlobalSpecialFXManager special_fx_manager_;
  GlobalRenderingManager global_rendering_manager_;
  GlobalSoundManager global_sound_manager_;
};



#endif