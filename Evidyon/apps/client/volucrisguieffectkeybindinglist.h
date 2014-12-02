//------------------------------------------------------------------------------------------------
// File:    volucrisguieffectkeybindinglist.h
//
// Desc:    
//
// Copyright (c) 2007-2008 Unseen Studios.   All rights reserved.
//------------------------------------------------------------------------------------------------
#ifndef __VOLUCRISGUIEFFECTKEYBINDINGLIST_H__
#define __VOLUCRISGUIEFFECTKEYBINDINGLIST_H__



class VolucrisGUIKeyBindingScreen;

/**
* Displays a list of effects
*/
class VolucrisGUIEffectKeyBindingList : public VolucrisGUISelectableTextLineList
{
public:

  /**
   * Gets the textual representation of the given key.  This is often more than 1 character.
   */
  static const char* getKeyText(GUIKey key);


protected:

  /**
   * Represents an element of the list
   */
  class EffectKeyBindingListEntry : public VolucrisListEntry {
  public:

    /**
     * Initializes this entry
     */
    EffectKeyBindingListEntry();

    /**
     * Called after the list entry has been set up in order to set
     * its displayed text.
     */
    void initializeListEntry(VolucrisGUIEffectKeyBindingList* list,
                             const ClientSpellDescription* effectToDisplay,
                             unsigned int effectIndex,
                             GUIKey currentKey);

    /**
     * Gets the effect that this index maps
     */
    unsigned int getEffectIndex() const;

    /**
     * Gets the key that is bound to this effect
     */
    GUIKey getKeyIndex() const;

    /**
     * Changes the key binding here.  Resets the "selected" state of this
     * element to deselected.
     */
    void setKeyIndex(GUIKey key);


  protected:

    /**
     * When this entry is selected, this method sends a notification to the
     * effect list to watch for keyboard input, then send the key pressed
     * to this list entry.
     */
    void onStateChange(StateChange stateChange);


  protected:

    /// Information about the effect is stored here
    const ClientSpellDescription* myEffectDescription;

    /// The effect that this list entry stores
    unsigned int myEffectIndex;

    /// The key index bound to this effect.  If this is GUIKEY_NULL, it is invalid.
    GUIKey myKey;

    /// The list that contains this class
    VolucrisGUIEffectKeyBindingList* myList;
  };

  /**
   * Watches for keyboard input when an effect is being bound
   */
  class KeyboardSentinel : public GUIKeyboardSentinel
  {
  public:

    /**
     * Initializes this sentinel with its owner
     */
    KeyboardSentinel(VolucrisGUIEffectKeyBindingList* owner);

    /**
     * Sets the next entry to bind.  If this method overwrites an entry, that entry
     * is deselected (since it's likely to be in the selected state).
     */
    void watchForBinding(EffectKeyBindingListEntry* nextEntryToBind);


  protected:

    /**
     * When a key is released, this method is invoked by the keyboard input source.  If the
     * next entry to bind has been set, it will bind the key, reset the entry and null out the
     * myNextEntryToBind pointer so that subsequent keys don't get assigned.
     * @return if (myNextEntryToBind) : 'true' to indicate the event should stop propogating;
               else 'false' because nothing happened
     */
    virtual bool onKeyUp(GUIKey key, GUIKeyboardInputSource* keyboard);


  protected:

    /// The entry that owns this sentinel
    VolucrisGUIEffectKeyBindingList* myOwner;

    /// The list entry that will be sent a binding notification next time a key is pressed
    EffectKeyBindingListEntry* myNextEntryToBind;
  };

public:

  /**
   * Initializes this class
   */
  VolucrisGUIEffectKeyBindingList();

  /**
   * Cleans up this class when it disappears to prevent memory leaks; however, it is
   * bad practice to rely on this method.
   */
  ~VolucrisGUIEffectKeyBindingList();

  /**
   * Sets up this list for use
   */
  bool create(GUIFont* font, GUILayer* layer);

  /**
   * Fills out the list using the given parameters
   */
  bool initialize(const unsigned int currentKeyToEffectMapping[256],
                   const SpellAvailabilityMask* effectsToShow,
                   const ClientSpellDescription* allEffectDescriptions, size_t numberOfEffectDescriptions);

  /**
   * Registers all of this item list's members with the target layer
   */
  bool show();

  /**
   * Unregisters all of this list's members from the target layer
   */
  void hide();

  /**
   * Erases members of this list from the given layer and cleans up internal references.
   */
  void destroy();

  /**
   * Erases all of the items in this list
   */
  void clear();

  /**
   * Constructs the list of key->effect mappings from the selections in this class
   */
  void buildKeyToEffectMappings(unsigned int keyToEffectMapping[256]);

  /**
   * Obtains this list's keyboard sentinel so that it can be registered with the input chain
   */
  GUIKeyboardSentinel* getKeyboardSentinel();


protected:

  /**
   * This method is called when a list entry is clicked so that the sentinel can be told
   * to bind the next key press to that entry.
   */
  void entryWantsToBeBound(EffectKeyBindingListEntry* entry);


protected:

  /**
   * Returns the number of lines in this list box
   */
  size_t getNumberOfLines() const;

  /**
   * Gets the list entry base class that corresponds to the given list item
   */
  ListEntry* getListEntry(size_t index);


protected:

  /// The sentinel that watches for and makes key bindings
  KeyboardSentinel myKeyboardSentinel;

  /// This is the layer into which this list renders
  GUILayer* myRenderingLayer;

  /// This font is passed to each of the list elements when they are created
  GUIFont* myFont;

  /// List of the entries in this class
  EffectKeyBindingListEntry* myEntries;

  /// How many entries are in the list
  size_t myNumberOfEntries;
};


#endif