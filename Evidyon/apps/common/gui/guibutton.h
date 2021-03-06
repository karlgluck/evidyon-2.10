//------------------------------------------------------------------------------------------------
// File:  guibutton.h
//
// Desc:  
//
// Copyright (c) 2007-2008 Unseen Studios.   All rights reserved.
//------------------------------------------------------------------------------------------------
#ifndef __GUIBUTTON_H__
#define __GUIBUTTON_H__

#include "guimouse.h"
#include "guitextlabelelement.h"

/**
 * This class creates a button that monitors mouse input on the screen.  This class
 * should be derived to display state changes, such as when the mouse moves over
 * the button.  Derived classes can also add button backgrounds and other interesting
 * effects.
 */
class GUIButton : public GUIComponent
{
  /**
   * This class is used by the GUIButton class to watch for input from the mouse
   */
  class MouseSentinel : public GUIMouseSentinel
  {
    public:

      /**
       * Initializes this button sentinel
       */
      MouseSentinel(GUIButton* button);


    protected:

      /**
       * Called by the mouse input source when the mouse moves
       * @return 'true' To indicate the event should stop propogating
       */
      bool onMouseMove(GUIPoint position, GUIPoint oldPosition, GUIMouseInputSource* mouse);

      /**
       * Called by the mouse input source when a mouse button alters its state
       * @return 'true' To indicate the event should stop propogating
       */
      bool onMouseButtonAction(GUIMouseButtonAction action, GUIPoint position, GUIMouseInputSource* mouse);


    protected:

      /// The button for which this sentinel watches for input
      GUIButton* myButton;

      /// Whether or not the mouse was over this button in the last update message
      bool myMouseOverFlag;
  };

  protected:


    /**
     * This parameter is passed by a button to its subclass in order to let it handle events
     */
    enum StateChange
    {
      /// Called when the mouse loses the ability to read input
      STATECHANGE_DISABLED,

      /// Called when the mouse enters the state when it is allowed to receive input
      STATECHANGE_ENABLED,

      /// Called when the mouse enters the state when the mouse's primary button is down and
      /// the cursor is positioned within the boundaries of the button
      STATECHANGE_MOUSEOVERANDDOWN,

      /// Called when the mouse enters the state when the mouse's primary button is up and
      /// the cursor is positioned within the boundaries of the button
      STATECHANGE_MOUSEOVER,

      /// Called when the mouse leaves this button's area
      STATECHANGE_MOUSEOUT,

      /// Called when this button is pushed
      STATECHANGE_PUSHED,

      /// Called after a pushed button becomes un-pushed
      STATECHANGE_RESET,
    };

  public:

    /**
     * Initializes this class
     */
    GUIButton();

    /**
     * Allow the subclass to clean up
     */
    virtual ~GUIButton();

    /**
     * Pushes this button
     */
    void push();

    /**
     * Determines whether or not this button was pushed
     */
    bool wasPushed() const;

    /**
     * Resets the button's pushed state to false.  This method should be called after
     * examining wasPushed() and the information is no longer needed.
     */
    void resetPushed();

    /**
     * Allows this button to respond to user input
     */
    void enable();

    /** 
     * Prevents this button from responding to user input
     */
    void disable();

    bool isEnabled() const;

    /**
     * Registers this button's visible elements with the given layer
     */
    virtual void registerElements(GUILayer* layer);

    /**
     * Unregisters this button's visible elements
     */
    virtual void unregisterElements(GUILayer* layer);

    /**
     * Gets the canvas for this button so that it can be positioned
     */
    //GUICanvas* getCanvas();

    /**
     * Gets this button's mouse sentinel so that it can be added to something which triggers its events
     */
    virtual GUIMouseSentinel* getMouseSentinel();


    GUITextLabelElement* getTextLabelElement();

  protected:

    /**
     * This method is invoked whenever the button changes state
     */
    virtual void onStateChange(StateChange stateChange);

    virtual void setClippingArea(const GUIRect* area);

  protected:


    /// This structure displays the button's text
    GUITextLabelElement myText;


  private:

    /// This structure watches for mouse input
    MouseSentinel myMouseSentinel;

    /// The flag that indicates whether or not this button has been pushed
    bool myPushedFlag;

    /// Flag that indicates whether or not this button can receive input. All
    /// buttons are enabled by default.
    bool myEnabledFlag;
};



#endif