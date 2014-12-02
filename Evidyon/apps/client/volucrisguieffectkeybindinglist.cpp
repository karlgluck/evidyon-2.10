//------------------------------------------------------------------------------------------------
// File:    volucrisguieffectkeybindinglist.cpp
//
// Desc:    
//
// Copyright (c) 2007-2008 Unseen Studios.   All rights reserved.
//------------------------------------------------------------------------------------------------
#include "stdafx.h"


//------------------------------------------------------------------------------------------------
// Name: getKeyText
// Desc: Gets the textual representation of the given key.  This is often more than 1 character.
//------------------------------------------------------------------------------------------------
const char* VolucrisGUIEffectKeyBindingList::getKeyText(GUIKey key)
{
    switch(key)
    {
        case GUIKEY_NULL:       return "(none)";
        case GUIKEY_ESCAPE:     return "ESCAPE";
        case GUIKEY_1:          return "1";
        case GUIKEY_2:          return "2";
        case GUIKEY_3:          return "3";
        case GUIKEY_4:          return "4";
        case GUIKEY_5:          return "5";
        case GUIKEY_6:          return "6";
        case GUIKEY_7:          return "7";
        case GUIKEY_8:          return "8";
        case GUIKEY_9:          return "9";
        case GUIKEY_0:          return "0";
        case GUIKEY_MINUS:      return "MINUS";
        case GUIKEY_EQUALS:     return "EQUALS";
        case GUIKEY_BACK:       return "BACK";
        case GUIKEY_TAB:        return "TAB";
        case GUIKEY_Q:          return "Q";
        case GUIKEY_W:          return "W";
        case GUIKEY_E:          return "E";
        case GUIKEY_R:          return "R";
        case GUIKEY_T:          return "T";
        case GUIKEY_Y:          return "Y";
        case GUIKEY_U:          return "U";
        case GUIKEY_I:          return "I";
        case GUIKEY_O:          return "O";
        case GUIKEY_P:          return "P";
        case GUIKEY_LBRACKET:   return "[";
        case GUIKEY_RBRACKET:   return "]";
        case GUIKEY_RETURN:     return "RETURN";
        case GUIKEY_LCONTROL:   return "LEFT CTRL"; //
        case GUIKEY_A:          return "A";
        case GUIKEY_S:          return "S";
        case GUIKEY_D:          return "D";
        case GUIKEY_F:          return "F";
        case GUIKEY_G:          return "G";
        case GUIKEY_H:          return "H";
        case GUIKEY_J:          return "J";
        case GUIKEY_K:          return "K";
        case GUIKEY_L:          return "L";
        case GUIKEY_SEMICOLON:  return "SEMICOLON";
        case GUIKEY_APOSTROPHE: return "APOSTROPHE";
        case GUIKEY_GRAVE:      return "GRAVE";
        case GUIKEY_LSHIFT:     return "LEFT SHIFT";    //
        case GUIKEY_BACKSLASH:  return "BACKSLASH";
        case GUIKEY_Z:          return "Z";
        case GUIKEY_X:          return "X";
        case GUIKEY_C:          return "C";
        case GUIKEY_V:          return "V";
        case GUIKEY_B:          return "B";
        case GUIKEY_N:          return "N";
        case GUIKEY_M:          return "M";
        case GUIKEY_COMMA:      return "COMMA";
        case GUIKEY_PERIOD:     return "PERIOD";
        case GUIKEY_SLASH:      return "FWD SLASH";     //
        case GUIKEY_RSHIFT:     return "RIGHT SHIFT";   //
        case GUIKEY_MULTIPLY:   return "MULTIPLY";
        case GUIKEY_LMENU:      return "LEFT ALT";      //
        case GUIKEY_SPACE:      return "SPACE";
        case GUIKEY_CAPITAL:    return "CAPITAL";
        case GUIKEY_F1:         return "F1";
        case GUIKEY_F2:         return "F2";
        case GUIKEY_F3:         return "F3";
        case GUIKEY_F4:         return "F4";
        case GUIKEY_F5:         return "F5";
        case GUIKEY_F6:         return "F6";
        case GUIKEY_F7:         return "F7";
        case GUIKEY_F8:         return "F8";
        case GUIKEY_F9:         return "F9";
        case GUIKEY_F10:        return "F10";
        case GUIKEY_NUMLOCK:    return "NUMLOCK";
        case GUIKEY_SCROLL:     return "SCROLL";
        case GUIKEY_NUMPAD7:    return "NUMPAD 7";  //
        case GUIKEY_NUMPAD8:    return "NUMPAD 8";  //
        case GUIKEY_NUMPAD9:    return "NUMPAD 9";  //
        case GUIKEY_SUBTRACT:   return "SUBTRACT";
        case GUIKEY_NUMPAD4:    return "NUMPAD 4";  //
        case GUIKEY_NUMPAD5:    return "NUMPAD 5";  //
        case GUIKEY_NUMPAD6:    return "NUMPAD 6";  //
        case GUIKEY_ADD:        return "ADD";
        case GUIKEY_NUMPAD1:    return "NUMPAD 1";  //
        case GUIKEY_NUMPAD2:    return "NUMPAD 2";  //
        case GUIKEY_NUMPAD3:    return "NUMPAD 3";  //
        case GUIKEY_NUMPAD0:    return "NUMPAD 0";  //
        case GUIKEY_DECIMAL:    return "DECIMAL";
        case GUIKEY_OEM_102:    return "OEM_102";
        case GUIKEY_F11:        return "F11";
        case GUIKEY_F12:        return "F12";
        case GUIKEY_F13:        return "F13";
        case GUIKEY_F14:        return "F14";
        case GUIKEY_F15:        return "F15";
        case GUIKEY_KANA:       return "KANA";
        case GUIKEY_ABNT_C1:    return "ABNT_C1";
        case GUIKEY_CONVERT:    return "CONVERT";
        case GUIKEY_NOCONVERT:  return "NOCONVERT";
        case GUIKEY_YEN:        return "YEN";
        case GUIKEY_ABNT_C2:    return "ABNT_C2";
        case GUIKEY_NUMPADEQUALS:return "NUMPAD ="; //
        case GUIKEY_PREVTRACK:  return "PREV. TRACK";   //
        case GUIKEY_AT:         return "AT";
        case GUIKEY_COLON:      return "COLON";
        case GUIKEY_UNDERLINE:  return "UNDERLINE";
        case GUIKEY_KANJI:      return "KANJI";
        case GUIKEY_STOP:       return "STOP";
        case GUIKEY_AX:         return "AX";
        case GUIKEY_UNLABELED:  return "UNLABELED";
        case GUIKEY_NEXTTRACK:  return "NEXTTRACK";
        case GUIKEY_NUMPADENTER:return "NUMPADENTER";
        case GUIKEY_RCONTROL:   return "RCONTROL";
        case GUIKEY_MUTE:       return "MUTE";
        case GUIKEY_CALCULATOR: return "CALCULATOR";
        case GUIKEY_PLAYPAUSE:  return "PLAYPAUSE";
        case GUIKEY_MEDIASTOP:  return "MEDIASTOP";
        case GUIKEY_VOLUMEDOWN: return "VOLUMEDOWN";
        case GUIKEY_VOLUMEUP:   return "VOLUMEUP";
        case GUIKEY_WEBHOME:    return "WEBHOME";
        case GUIKEY_NUMPADCOMMA:return "NUMPADCOMMA";
        case GUIKEY_DIVIDE:     return "DIVIDE";
        case GUIKEY_SYSRQ:      return "SYSRQ";
        case GUIKEY_RMENU:      return "RIGHT ALT"; //
        case GUIKEY_PAUSE:      return "PAUSE";
        case GUIKEY_HOME:       return "HOME";
        case GUIKEY_UP:         return "UP";
        case GUIKEY_PRIOR:      return "PRIOR";
        case GUIKEY_LEFT:       return "LEFT";
        case GUIKEY_RIGHT:      return "RIGHT";
        case GUIKEY_END:        return "END";
        case GUIKEY_DOWN:       return "DOWN";
        case GUIKEY_NEXT:       return "NEXT";
        case GUIKEY_INSERT:     return "INSERT";
        case GUIKEY_DELETE:     return "DELETE";
        case GUIKEY_LWIN:       return "LEFT WIN";  //
        case GUIKEY_RWIN:       return "RIGHT WIN"; //
        case GUIKEY_APPS:       return "APPS";
        case GUIKEY_POWER:      return "POWER";
        case GUIKEY_SLEEP:      return "SLEEP";
        case GUIKEY_WAKE:       return "WAKE";
        case GUIKEY_WEBSEARCH:  return "WEBSEARCH";
        case GUIKEY_WEBFAVORITES:return "WEBFAVORITES";
        case GUIKEY_WEBREFRESH: return "WEBREFRESH";
        case GUIKEY_WEBSTOP:    return "WEBSTOP";
        case GUIKEY_WEBFORWARD: return "WEBFORWARD";
        case GUIKEY_WEBBACK:    return "WEBBACK";
        case GUIKEY_MYCOMPUTER: return "MYCOMPUTER";
        case GUIKEY_MAIL:       return "MAIL";
        case GUIKEY_MEDIASELECT:return "MEDIASELECT";
        default:    return "???";
    }
}



//------------------------------------------------------------------------------------------------
// Name: VolucrisGUIEffectKeyBindingList
// Desc: Initializes this class with its owning key binding screen
//------------------------------------------------------------------------------------------------
VolucrisGUIEffectKeyBindingList::VolucrisGUIEffectKeyBindingList()
    : myKeyboardSentinel(this)
{
    // TODO: zero()
    myRenderingLayer = 0;
    myFont = 0;
    myEntries = 0;
    myNumberOfEntries = 0;

    setLineHeight(30);
}



//------------------------------------------------------------------------------------------------
// Name: ~VolucrisGUIEffectKeyBindingList
// Desc: Cleans up this class when it disappears to prevent memory leaks; however, it is
//       bad practice to rely on this method.
//------------------------------------------------------------------------------------------------
VolucrisGUIEffectKeyBindingList::~VolucrisGUIEffectKeyBindingList()
{
    destroy();
}



//------------------------------------------------------------------------------------------------
// Name: create
// Desc: Sets up this list for use
//------------------------------------------------------------------------------------------------
bool VolucrisGUIEffectKeyBindingList::create(GUIFont* font, GUILayer* layer)
{
    // Fail unless all parameters are valid
    if (!font || !layer) return false;

    // Save the variables
    myFont = font;
    myRenderingLayer = layer;

    // Success
    return true;
}



//------------------------------------------------------------------------------------------------
// Name: initialize
// Desc: Fills out the list using the given parameters
//------------------------------------------------------------------------------------------------
bool VolucrisGUIEffectKeyBindingList::initialize(const unsigned int currentKeyToEffectMapping[256],
    const SpellAvailabilityMask* effectsToShow,
    const ClientSpellDescription* allEffectDescriptions, size_t numberOfEffectDescriptions)
{
    // Erase any existing elements
    clear();

    if (!currentKeyToEffectMapping || !effectsToShow || !allEffectDescriptions) return true;

    // Parse the effectsToShow mask to get the number of elements we'll need
    size_t numberOfEntries = 0;
    for (int i = 0; i < MAX_SPELL_TYPES; ++i)
        if (effectsToShow->isBitSet(i)) ++numberOfEntries;

    // Allocate the array of elements
    myEntries = new EffectKeyBindingListEntry[numberOfEntries];
    myNumberOfEntries = numberOfEntries;

    DEBUG_INFO("Showing %lu effects in key binding screen!", numberOfEntries);

    // Build a reversed list of effect->key mappings
    GUIKey effectToKeyMapping[MAX_SPELL_TYPES];
    ZeroMemory(effectToKeyMapping, sizeof(effectToKeyMapping));
    for (int i = 0; i < 256; ++i)
    {
        unsigned int currentEffect = currentKeyToEffectMapping[i];
        if (currentEffect < MAX_SPELL_TYPES)
            effectToKeyMapping[currentEffect] = (GUIKey)i;
    }

    // Fill in each of the entries
    size_t listIndex = 0;
    for (int i = 0; i < MAX_SPELL_TYPES && listIndex < numberOfEntries; ++i)
    {
        if (effectsToShow->isBitSet(i))
        {
            EffectKeyBindingListEntry* entry = &myEntries[listIndex++];
            entry->setFont(myFont);

            entry->initializeListEntry(this,
                                      &allEffectDescriptions[i],
                                       i,
                                       effectToKeyMapping[i]);
        }
    }

    // Refresh this list
    refresh();

    // Success
    return true;
}



//------------------------------------------------------------------------------------------------
// Name: show
// Desc: Registers all of this item list's members with the target layer
//------------------------------------------------------------------------------------------------
bool VolucrisGUIEffectKeyBindingList::show()
{
    // Don't bother doing anything if the rendering layer is invalid
    if (!myRenderingLayer) return false;
    registerElements(myRenderingLayer);

    // If the item list doesn't exist, return early
    if (!myEntries) return true;
    for (size_t i = 0; i < myNumberOfEntries; ++i)
        myEntries[i].registerElements(myRenderingLayer);
    
    // Success
    return true;
}



//------------------------------------------------------------------------------------------------
// Name: hide
// Desc: Unregisters all of this list's members from the target layer
//------------------------------------------------------------------------------------------------
void VolucrisGUIEffectKeyBindingList::hide()
{
    if (!myRenderingLayer) return;
    unregisterElements(myRenderingLayer);

    if (!myEntries) return;
    for (size_t i = 0; i < myNumberOfEntries; ++i)
        myEntries[i].unregisterElements(myRenderingLayer);
}



//------------------------------------------------------------------------------------------------
// Name: destroy
// Desc: Erases members of this list from the given layer and cleans up internal references.
//------------------------------------------------------------------------------------------------
void VolucrisGUIEffectKeyBindingList::destroy()
{
    // Erase elements in this list
    clear();

    // TODO: zero()
    myRenderingLayer = 0;
    myFont = 0;
    myEntries = 0;
    myNumberOfEntries = 0;
}



//------------------------------------------------------------------------------------------------
// Name: clear
// Desc: Erases all of the items in this list
//------------------------------------------------------------------------------------------------
void VolucrisGUIEffectKeyBindingList::clear()
{
    // Remove all of the elements from the GUI
    hide();

    // Erase the list of entries
    if (myEntries)
    {
        // Get rid of the list
        delete [] myEntries;
        myEntries = 0;
        myNumberOfEntries = 0;
    }

    // Update the display
    refresh();
}



//------------------------------------------------------------------------------------------------
// Name: buildKeyToEffectMappings
// Desc: Constructs the list of key->effect mappings from the selections in this class
//------------------------------------------------------------------------------------------------
void VolucrisGUIEffectKeyBindingList::buildKeyToEffectMappings(unsigned int keyToEffectMapping[256])
{
  if (keyToEffectMapping == NULL) return;

  // Reset the output mapping
  memset(keyToEffectMapping, 0xFFFFFFFF, sizeof(unsigned int) * 256);

  if (myEntries == NULL) return;

  // Get the mapping of this entry
  for (size_t i = 0; i < myNumberOfEntries; ++i) {
    GUIKey key = myEntries[i].getKeyIndex();
    if (key < GUIKEY_COUNT) {
      keyToEffectMapping[key] = myEntries[i].getEffectIndex();
    }
  }
}



//------------------------------------------------------------------------------------------------
// Name: getKeyboardSentinel
// Desc: Obtains this list's keyboard sentinel so that it can be registered with the input chain
//------------------------------------------------------------------------------------------------
GUIKeyboardSentinel* VolucrisGUIEffectKeyBindingList::getKeyboardSentinel()
{
    return &myKeyboardSentinel;
}



//------------------------------------------------------------------------------------------------
// Name: entryWantsToBeBound
// Desc: This method is called when a list entry is clicked so that the sentinel can be told
//       to bind the next key press to that entry.
//------------------------------------------------------------------------------------------------
void VolucrisGUIEffectKeyBindingList::entryWantsToBeBound(EffectKeyBindingListEntry* entry)
{
    //entry->setKeyIndex(GUIKEY_NULL);
    myKeyboardSentinel.watchForBinding(entry);
}



//------------------------------------------------------------------------------------------------
// Name: getNumberOfLines
// Desc: Returns the number of lines in this list box
//------------------------------------------------------------------------------------------------
size_t VolucrisGUIEffectKeyBindingList::getNumberOfLines() const
{
    return myNumberOfEntries;
}



//------------------------------------------------------------------------------------------------
// Name: getListEntry
// Desc: Gets the list entry base class that corresponds to the given list item
//------------------------------------------------------------------------------------------------
GUIList::ListEntry* VolucrisGUIEffectKeyBindingList::getListEntry(size_t index)
{
    return (ListEntry*)&(myEntries[index]);
}



//------------------------------------------------------------------------------------------------
// Name: KeyboardSentinel
// Desc: Initializes this sentinel with its owner
//------------------------------------------------------------------------------------------------
VolucrisGUIEffectKeyBindingList::KeyboardSentinel::KeyboardSentinel(VolucrisGUIEffectKeyBindingList* owner)
{
    myOwner = owner;
    myNextEntryToBind = 0;
}



//------------------------------------------------------------------------------------------------
// Name: watchForBinding
// Desc: Sets the next entry to bind.  If this method overwrites an entry, that entry
//       is deselected (since it's likely to be in the selected state).
//------------------------------------------------------------------------------------------------
void VolucrisGUIEffectKeyBindingList::KeyboardSentinel::watchForBinding(EffectKeyBindingListEntry* nextEntryToBind)
{
    if (myNextEntryToBind)
        myNextEntryToBind->deselect();
    myNextEntryToBind = nextEntryToBind;
}



//------------------------------------------------------------------------------------------------
// Name: onKeyUp
// Desc: When a key is released, this method is invoked by the keyboard input source.  If the
//       next entry to bind has been set, it will bind the key, reset the entry and null out the
//       myNextEntryToBind pointer so that subsequent keys don't get assigned.
//       @return if (myNextEntryToBind) : 'true' to indicate the event should stop propogating;
//       else 'false' because nothing happened
//------------------------------------------------------------------------------------------------
bool VolucrisGUIEffectKeyBindingList::KeyboardSentinel::onKeyUp(GUIKey key, GUIKeyboardInputSource* keyboard)
{
    if (myNextEntryToBind)
    {
        if (key == GUIKEY_DELETE && myNextEntryToBind->getKeyIndex() != GUIKEY_NULL)
          myNextEntryToBind->setKeyIndex(GUIKEY_NULL);
        else
          myNextEntryToBind->setKeyIndex(key);
        myNextEntryToBind->deselect();
        myNextEntryToBind = 0;

        // Don't propogate
        return true;
    }
    else
        return false;
}



//------------------------------------------------------------------------------------------------
// Name: EffectKeyBindingListEntry
// Desc: Initializes this entry
//------------------------------------------------------------------------------------------------
VolucrisGUIEffectKeyBindingList::EffectKeyBindingListEntry::EffectKeyBindingListEntry()
{
    myEffectDescription = 0;
    myEffectIndex = 0;
    myKey = GUIKEY_NULL;
    myList = 0;
}



//------------------------------------------------------------------------------------------------
// Name: initializeListEntry
// Desc: Called after the list entry has been set up in order to set
//       its displayed text.
//------------------------------------------------------------------------------------------------
void VolucrisGUIEffectKeyBindingList::EffectKeyBindingListEntry::initializeListEntry(
    VolucrisGUIEffectKeyBindingList* list,
    const ClientSpellDescription* effectToDisplay, unsigned int effectIndex, GUIKey currentKey)
{
    myEffectDescription = effectToDisplay;
    myEffectIndex = effectIndex;
    myList = list;

    // Save the key and update the text
    setKeyIndex(currentKey);
}



//------------------------------------------------------------------------------------------------
// Name: getEffectIndex
// Desc: Gets the effect that this index maps
//------------------------------------------------------------------------------------------------
unsigned int VolucrisGUIEffectKeyBindingList::EffectKeyBindingListEntry::getEffectIndex() const
{
    return myEffectIndex;
}



//------------------------------------------------------------------------------------------------
// Name: getKeyIndex
// Desc: Gets the key that is bound to this effect
//------------------------------------------------------------------------------------------------
GUIKey VolucrisGUIEffectKeyBindingList::EffectKeyBindingListEntry::getKeyIndex() const
{
    return myKey;
}



//------------------------------------------------------------------------------------------------
// Name: setKeyIndex
// Desc: Changes the key binding here.  Resets the "selected" state of this
//       element to deselected.
//------------------------------------------------------------------------------------------------
void VolucrisGUIEffectKeyBindingList::EffectKeyBindingListEntry::setKeyIndex(GUIKey key)
{
    myKey = key;

    const char* keyText = VolucrisGUIEffectKeyBindingList::getKeyText(myKey);
    std::string label = keyText;
    if (label.length() < 8)
      label = std::string("        ").substr(0,8-label.length()).append(label);
    label.append(" : ");
    label.append(myEffectDescription->name);

    // Set the name of this line based on the key mapping and effect
    setText(label.c_str());
}



//------------------------------------------------------------------------------------------------
// Name: onStateChange
// Desc: When this entry is selected, this method sends a notification to the
//       effect list to watch for keyboard input, then send the key pressed
//       to this list entry.
//------------------------------------------------------------------------------------------------
void VolucrisGUIEffectKeyBindingList::EffectKeyBindingListEntry::onStateChange(StateChange stateChange)
{
    // Only process messages if the list has been initialized
    if (!myList) return;

    // Change the entry being bound
    if (stateChange == STATECHANGE_SELECTED)
        myList->entryWantsToBeBound(this);
}

