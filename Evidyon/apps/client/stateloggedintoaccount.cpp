//------------------------------------------------------------------------------------------------
// File:    statenoconnection.cpp
//
// Desc:    
//
// Copyright (c) 2007-2008 Unseen Studios.   All rights reserved.
//------------------------------------------------------------------------------------------------
#include "stdafx.h"
#include "volucrisguidefs.h"


struct CharacterLoginInfo {
    unsigned int id;
    char name[MAX_AVATAR_NAME_LENGTH + 50];
    float world_x, world_y;
    unsigned int actor_type;
    size_t visible_equipment[ACTORATTACHMENTPOINT_COUNT];
};

class VolucrisGUICharacterList : public VolucrisGUIList {

    class VolucrisCharacterListEntry : public VolucrisGUIList::VolucrisListEntryWithMouseInput {

        public:

            VolucrisCharacterListEntry();
            void setOwner(VolucrisGUICharacterList* owner) { owner_ = owner; }
            void setTextFont(GUIFont* font);
            bool getSelected() const;
            void setText(const char* text) { myText.setText(text); myText.setTextColor(VolucrisGUIDefs::CONTENT_COMPONENT_TEXT_COLOR); }
            void setID(unsigned int id) { id_ = id; }
            unsigned int getID() const { return id_; }
            void setSelected(bool selected) {
                selected_ = selected;
                myBackground.setType(selected ? VolucrisGUIListEntryBackground::DARK : VolucrisGUIListEntryBackground::LIGHT);
                myText.setTextColor(VolucrisGUIDefs::CONTENT_COMPONENT_TEXT_COLOR);
            }
        public:

            virtual void registerElements(GUILayer* layer);
            virtual void unregisterElements(GUILayer* layer);
            virtual void refresh();


        protected:

            virtual void onMouseAction(MouseAction action);

        protected:

            VolucrisGUICharacterList* owner_;

            /// Whether or not this element is selected
            bool selected_;

            /// This character's ID number
            unsigned int id_;

            /// The background GUI elmeent
            VolucrisGUIListEntryBackground myBackground;

            /// The text shown in the line
            VolucrisGUIListEntryText myText;
    };

    public:

        void create(const CharacterLoginInfo character_info[MAX_AVATARS_PER_ACCOUNT], unsigned int characters,
                     GUIFont* font, GUILayer* layer, GUIMouseInputSource* input) {
            setCharacters(character_info, characters);
            setTextFont(font);
            registerElements(layer);

            selection_changed_ = false;

            background_canvas_.setSourceCanvas(layer->getPrimaryCanvas());
            background_canvas_.absoluteWidth(300);
            background_canvas_.absoluteHeight(150);
            background_canvas_.alignXCenter(0);
            background_canvas_.alignYCenter(+135);
            background_canvas_.absoluteDepth(0.5f);

            myTargetCanvas.setSourceCanvas(&background_canvas_);
            myTargetCanvas.relativeWidth(-16);
            myTargetCanvas.relativeHeight(-16);
            myTargetCanvas.alignXCenter(0);
            myTargetCanvas.alignYCenter(0);
            myTargetCanvas.relativeDepth(-0.1f);

            background_.setTexture(VolucrisGUIDefs::GUI_TEXTURE_INDEX);
            background_.setSourceCanvas(&background_canvas_);
            background_.setSourceRegion(&VolucrisGUIDefs::CONTENT_COMPONENT_BACKGROUND,4, VolucrisGUIDefs::TEXTURE_SIZE);
            layer->registerElement(&background_);

            setLineHeight(26);

            GUIList::getMouseSentinel()->setParentMouseSentinel(input->getPrimarySentinel());

            // Refresh this list
            refresh();
        }

        void destroy(GUILayer* layer) {
            unregisterElements(layer);
            layer->unregisterElement(&background_);
        }

        void setTextFont(GUIFont* font) {
            for (unsigned int i = 0; i < MAX_AVATARS_PER_ACCOUNT; ++i) {
                myCharacters[i].setTextFont(font);
            }
        }

        /**
         * Invoked by the mouse sentinel when the mouse leaves or enters the
         * list.
         */
        //virtual void onMouseCrossedListBorder(bool inside);

        void setCharacters(const CharacterLoginInfo* characters, unsigned int number) {
            number_of_characters_ = number;
            for (unsigned int i = 0; i < number; ++i) {
                myCharacters[i].setOwner(this);
                myCharacters[i].setID(characters[i].id);
                myCharacters[i].setText(characters[i].name);
            }
        }

        unsigned int getSelection() {
            for (unsigned int i = 0; i < number_of_characters_; ++i) {
                if (myCharacters[i].getSelected()) return i;
            }
            return 0xFFFFFFFF;
        }

        void setSelection(unsigned int selection) {
            selection_changed_ = true;
            for (unsigned int i = 0; i < number_of_characters_; ++i) {
                myCharacters[i].setSelected(i == selection);
            }
        }

        bool selectionChanged() {
            bool return_value = selection_changed_;
            selection_changed_ = false;
            return return_value;
        }

        GUICanvas* getCanvas() { return &myTargetCanvas; }

    protected:

        void setSelection(VolucrisCharacterListEntry* this_pointer) {
            setSelection((this_pointer >= myCharacters &&
                           this_pointer < myCharacters + MAX_AVATARS_PER_ACCOUNT) ?
                            this_pointer - myCharacters : 0xFFFFFFFF);
        }

    protected:

        GUICanvas background_canvas_;
        GUIBorderedFillElement background_;
        GUIList::ListEntry* getListEntry(unsigned int index) { return index < number_of_characters_ ? &myCharacters[index] : NULL; }
        unsigned int getNumberOfLines() const                  { return number_of_characters_; }

        bool selection_changed_;
        unsigned int number_of_characters_;
        VolucrisCharacterListEntry myCharacters[MAX_AVATARS_PER_ACCOUNT];
};


//------------------------------------------------------------------------------------------------
// Name: ItemListEntry
// Desc: Initializes this class
//------------------------------------------------------------------------------------------------
VolucrisGUICharacterList::VolucrisCharacterListEntry::VolucrisCharacterListEntry()
: myBackground(this), myText(this)
{
    myText.setTextColor(VolucrisGUIDefs::CONTENT_COMPONENT_TEXT_COLOR);
    selected_ = false;
}



//------------------------------------------------------------------------------------------------
// Name: setTextFont
// Desc: Sets the font used to display the text in this list entry
//------------------------------------------------------------------------------------------------
void VolucrisGUICharacterList::VolucrisCharacterListEntry::setTextFont(GUIFont* font)
{
    myText.setFont(font);
}



//------------------------------------------------------------------------------------------------
bool VolucrisGUICharacterList::VolucrisCharacterListEntry::getSelected() const {
  return selected_;
}


//------------------------------------------------------------------------------------------------
void VolucrisGUICharacterList::VolucrisCharacterListEntry::registerElements(GUILayer* layer)
{
    myText.registerElements(layer);
    myBackground.registerElements(layer);
}


//------------------------------------------------------------------------------------------------
void VolucrisGUICharacterList::VolucrisCharacterListEntry::unregisterElements(GUILayer* layer)
{
    myText.unregisterElements(layer);
    myBackground.unregisterElements(layer);
}



//------------------------------------------------------------------------------------------------
// Name: setClippingArea
// Desc: Sets the clipping region of the internal elements
//------------------------------------------------------------------------------------------------
void VolucrisGUICharacterList::VolucrisCharacterListEntry::refresh()
{
    myText.setClippingArea(getClippingArea());
    //myText.getCanvas()->realign();
    myBackground.setClippingArea(getClippingArea());
    //myBackground.getCanvas()->realign();
}


void VolucrisGUICharacterList::VolucrisCharacterListEntry::onMouseAction(
    VolucrisGUIList::VolucrisListEntryWithMouseInput::MouseAction action) {
  switch(action) {
    case MOUSEOVER:         if (!selected_) myBackground.setType(VolucrisGUIListEntryBackground::MEDIUM); break;
    case MOUSEOVERANDDOWN:  myBackground.setType(VolucrisGUIListEntryBackground::DARK);   break;
    case CLICKED:           owner_->setSelection(selected_ ? 0 : this);         break;
    case MOUSEOUT:          myBackground.setType(selected_ ? VolucrisGUIListEntryBackground::DARK : VolucrisGUIListEntryBackground::LIGHT); break;
  }
}








//------------------------------------------------------------------------------------------------
// Name:  stateLoggedIntoAccount
// Desc:  
//------------------------------------------------------------------------------------------------
void VolucrisClient::stateLoggedIntoAccount()
{
using namespace ::Evidyon::Network::Packet;
using namespace ::Evidyon::Network::Packet::Client;
    global_actor_manager_.reset();

    CharacterLoginInfo character_info[MAX_AVATARS_PER_ACCOUNT];


    // Account information
    float daysLeft;
    unsigned int characterIDs[MAX_AVATARS_PER_ACCOUNT];
    size_t numCharacterIDs;

    // Get a list of characters and other account information from the server
    bool gotAccountInfo = false;
    ENetEvent netEvent;
    enet_peer_send(myENetServer, 0, ::Evidyon::Network::Packet::Client::Encode::askAccountInfo());
    gotAccountInfo = false;
    unsigned long startTime = dcxWin32Clock::getSystemTime();
    while(dcxWin32Clock::getSystemTime() - startTime < 1000 &&
     !(gotAccountInfo = (enet_host_service(myENetClient, &netEvent, 1000) > 0 && netEvent.type == ENET_EVENT_TYPE_RECEIVE && netEvent.channelID == 0)));
    if (gotAccountInfo)
    {
        ::Evidyon::Network::Packet::Message msg = ::Evidyon::Network::Packet::NETMSG_INVALID;
        ::Evidyon::Network::Packet::decodeMessageType(netEvent.packet, &msg);
        gotAccountInfo = msg == ::Evidyon::Network::Packet::NETMSG_TELLACCOUNTINFO;
    }
    if (gotAccountInfo)
    {
        gotAccountInfo = !APP_WARNING(!Decode::tellAccountInfo(netEvent.packet, &daysLeft, characterIDs, &numCharacterIDs))
                            ("Failed to decode tellAccountInfo");
    }
    if (netEvent.packet) enet_packet_destroy(netEvent.packet);
    if (!gotAccountInfo)
    {
        myStateMachine.queueState(VCS_DISCONNECT);
        myStateMachine.queueState(VCS_NO_CONNECTION);
        return;
    }

    // Get each of the characters' names
    // TODO: use an internal database to look up character names! :D
    char characterNames[5][MAX_AVATAR_NAME_LENGTH + 50]; // 8-26-08: character name extended...
    for (size_t i = 0; i < numCharacterIDs; ++i)
    {
        enet_peer_send(myENetServer, 0, ::Evidyon::Network::Packet::Client::Encode::askCharacterName(characterIDs[i]));
        unsigned long startTime = dcxWin32Clock::getSystemTime();
        bool gotCharacterName = false;
        while(dcxWin32Clock::getSystemTime() - startTime < 1000 &&
             !(gotCharacterName = (enet_host_service(myENetClient, &netEvent, 1000) > 0 && netEvent.type == ENET_EVENT_TYPE_RECEIVE && netEvent.channelID == 0)));
        if (gotCharacterName)
        {
            ::Evidyon::Network::Packet::Message msg = ::Evidyon::Network::Packet::NETMSG_INVALID;
            ::Evidyon::Network::Packet::decodeMessageType(netEvent.packet, &msg);
            gotAccountInfo = msg == ::Evidyon::Network::Packet::NETMSG_TELLCHARACTERNAME;
        }
        if (gotCharacterName)
        {
            unsigned int id;
            gotCharacterName = !APP_WARNING(!Decode::tellCharacterName(
                netEvent.packet, &id, characterNames[i], MAX_AVATAR_NAME_LENGTH,
                &character_info[i].world_x, &character_info[i].world_y,
                &character_info[i].actor_type, character_info[i].visible_equipment))
                                ("Failed to decode tellCharacterName");
            gotCharacterName = id == characterIDs[i];
        }

        enet_packet_destroy(netEvent.packet);

        if (!gotCharacterName)
        {
            myStateMachine.queueState(VCS_DISCONNECT);
            myStateMachine.queueState(VCS_NO_CONNECTION);
            return;
        }
    }
    for (size_t i = numCharacterIDs; i < MAX_AVATARS_PER_ACCOUNT; ++i)
    {
        strcpy_s(characterNames[i], MAX_AVATAR_NAME_LENGTH + 50, "* click to create character *");
    }

    for (size_t i = 0; i < MAX_AVATARS_PER_ACCOUNT; ++i)
    {
        strcpy_s(character_info[i].name, MAX_AVATAR_NAME_LENGTH+50, characterNames[i]);
        character_info[i].id = characterIDs[i];
    }

#define GUIGROUP_COMMON 0
#define BUTTON_CHARACTER(i) (50+i)
#define BUTTON_LOGOUT   1
#define TEXT_DAYSLEFT   2

    char daysLeftText[64];
    //sprintf_s(daysLeftText, sizeof(daysLeftText), "%.02f days left", daysLeft);
    sprintf_s(daysLeftText, sizeof(daysLeftText), "unlimited days left");


    const char* names[5] = { characterNames[0], characterNames[1], characterNames[2], characterNames[3], characterNames[4] };
    VolucrisGUICharacterList characterList;
    characterList.create(
      character_info, numCharacterIDs, &myWindowFont,
      &gui_primary_layer_, &myMouseInputSource);

    VolucrisGUIButton createCharacter, enterWorld, deleteCharacter, logout;
    createCharacter.getCanvas()->setSourceCanvas(gui_primary_layer_.getPrimaryCanvas());
    createCharacter.getCanvas()->alignXCenter(0);
    createCharacter.getCanvas()->alignYCenter(+235);
    createCharacter.getCanvas()->absoluteWidth(320);
    createCharacter.getCanvas()->absoluteHeight(35);
    createCharacter.getCanvas()->absoluteDepth(0.5f);
    createCharacter.setFont(&myWindowFont);
    createCharacter.registerElements(&gui_primary_layer_);
    createCharacter.setText("Create Character");
    createCharacter.getMouseSentinel()->setParentMouseSentinel(myMouseInputSource.getPrimarySentinel());

    if (numCharacterIDs >= MAX_AVATARS_PER_ACCOUNT)
        createCharacter.disable();

    logout.getCanvas()->setSourceCanvas(gui_primary_layer_.getPrimaryCanvas());
    logout.getCanvas()->alignXCenter(+0);
    logout.getCanvas()->alignYCenter(270);
    logout.getCanvas()->absoluteWidth(320);
    logout.getCanvas()->absoluteHeight(35);
    logout.getCanvas()->absoluteDepth(0.5f);
    logout.setFont(&myWindowFont);

    logout.registerElements(&gui_primary_layer_);
    logout.setText("Log Out");
    logout.getMouseSentinel()->setParentMouseSentinel(myMouseInputSource.getPrimarySentinel());

    enterWorld.getCanvas()->setSourceCanvas(gui_primary_layer_.getPrimaryCanvas());
    enterWorld.getCanvas()->alignXCenter(-200);
    enterWorld.getCanvas()->alignYTop(0);
    enterWorld.getCanvas()->absoluteWidth(220);
    enterWorld.getCanvas()->absoluteHeight(35);
    enterWorld.getCanvas()->absoluteDepth(0.4f);
    enterWorld.setFont(&myWindowFont);

    enterWorld.setText("Enter World");

    deleteCharacter.getCanvas()->setSourceCanvas(gui_primary_layer_.getPrimaryCanvas());
    deleteCharacter.getCanvas()->alignXCenter(+200);
    deleteCharacter.getCanvas()->alignYTop(0);
    deleteCharacter.getCanvas()->absoluteWidth(220);
    deleteCharacter.getCanvas()->absoluteHeight(35);
    deleteCharacter.getCanvas()->absoluteDepth(0.4f);
    deleteCharacter.setFont(&myWindowFont);

    deleteCharacter.setText("Delete");

    enterWorld.getCanvas()->alignYCenter(0);
    deleteCharacter.getCanvas()->alignYCenter(0);

    //confirmDeleteCharacter.getCanvas()->setSourceCanvas(gui_primary_layer_.getPrimaryCanvas());
    //confirmDeleteCharacter.getCanvas()->alignXCenter(0);
    //confirmDeleteCharacter.getCanvas()->alignYCenter(-100);
    //confirmDeleteCharacter.getCanvas()->absoluteWidth(220);
    //confirmDeleteCharacter.getCanvas()->absoluteHeight(35);
    //confirmDeleteCharacter.getCanvas()->absoluteDepth(0.4f);
    //confirmDeleteCharacter.setFont(&myWindowFont);
    //confirmDeleteCharacter.setText("Complete Deletion");
    //confirmDeleteCharacter.getMouseSentinel()->setParentMouseSentinel(myMouseInputSource.getPrimarySentinel());



    float current_x = 0.0f, current_y = 0.0f;


    // If the window closes, exit
    myStateMachine.jumpState(VCS_DISCONNECT);
    myStateMachine.queueState(VCS_SHUTDOWN);

    // The actively selected character
    Actor* internalActor = 0;

    double lastRenderedFrameTime = dcxWin32Clock::getAccurateSystemTime();

    bool lastSelectionWasValid = false;




    // Loop until we get a reply
    while(windowsMessagePump())
    {
        // Poll the connection to keep it alive
        // TODO: make message reception asynchronous and actually use this method
        {
            ENetEvent netEvent;
            enet_host_service(myENetClient, &netEvent, 0);
        }

        double currentTime = dcxWin32Clock::getAccurateSystemTime();

        {
            //if (buttonClicked >= BUTTON_CHARACTER(0) && buttonClicked <= BUTTON_CHARACTER(4))
            if (enterWorld.wasPushed())
            {
                enterWorld.resetPushed();

                int character = characterList.getSelection();//buttonClicked - BUTTON_CHARACTER(0);
                if (character < numCharacterIDs)
                {
                    // Log in to the world with this character
                    bool enteredWorld;
                    {
                        ActorID actorID;
                        enet_peer_send(myENetServer, 0, ::Evidyon::Network::Packet::Client::Encode::enterWorld(characterIDs[character]));
                        global_map_manager_.update(character_info[character].world_x, character_info[character].world_y);
                        enteredWorld = false;
                        unsigned long startTime = dcxWin32Clock::getSystemTime();
                        while(dcxWin32Clock::getSystemTime() - startTime < 1000 &&
                             !(enteredWorld = (enet_host_service(myENetClient, &netEvent, 1000) > 0 && netEvent.type == ENET_EVENT_TYPE_RECEIVE && netEvent.channelID == 0)));
                        if (enteredWorld)
                        {
                            ::Evidyon::Network::Packet::Message msg = ::Evidyon::Network::Packet::NETMSG_INVALID;
                            ::Evidyon::Network::Packet::decodeMessageType(netEvent.packet, &msg);
                            enteredWorld = msg == ::Evidyon::Network::Packet::NETMSG_ENTERWORLD_SUCCEEDED;
                        }

                        // Used to obtain the world-location from the message
                        float x, y;
                        int map_index;
                        unsigned long time_of_day;
                        if (enteredWorld) {
                          enteredWorld = !APP_WARNING(!Decode::enterWorldSucceeded(
                                netEvent.packet, &actorID, &time_of_day, &x, &y, &map_index, &myCharacter.male, &myCharacter.raceIndex,
                                &myCharacter.classIndex, &myCharacter.currentHP, &myCharacter.maxHP, &myCharacter.currentMP,
                                &myCharacter.maxMP, &myCharacter.money, &myCharacter.geonite, &myCharacter.effectAvailability))
                                                    ("Failed to decode enterWorldSucceeded");

                            unsigned int t;
                            UnpackActorID(actorID, &t);

                            // Get the player's name
                            memcpy(myCharacter.name, characterNames[character], MAX_AVATAR_NAME_LENGTH);
                        }

                        // Subtract this number of seconds to get the true time
                        time_of_day_adjustment_ = dcx::dcxWin32Clock::getAccurateSystemTime() - time_of_day / 1000.0;

                        // Move to the next game state
                        if (enteredWorld)
                        {
                            //DEBUG_INFO("\nLogged into actor %i:\n:x, y\t=%.02f, %.02f\ngender:\t%s\nrace:\t%s\nclass:\t%s\nhp:\t%i/%i\nmp:\t%i/%i\n",
                            //            actorID, x, y,
                            //            myCharacter.male ? "Male" : "Female",
                            //            myResources.avatars.raceArray[myCharacter.raceIndex].name.c_str(),
                            //            myResources.avatars.classArray[myCharacter.classIndex].name.c_str(),
                            //            myCharacter.currentHP, myCharacter.maxHP,
                            //            myCharacter.currentMP, myCharacter.maxMP);

                            myStateMachine.jumpState(VCS_WORLD_MAIN);

                            // Get rid of any actors in the table
                            global_actor_manager_.reset();
                            special_fx_manager_.clear();
                            unsigned int new_background_loop = 0;
                            global_map_manager_.changeMap(map_index, &new_background_loop);
                            global_sound_manager_.loopBackgroundSound(new_background_loop);

                            // Create an actor for this client
                            global_actor_manager_.acquireClientActor(actorID, x, y, myCharacter.name);
                        }
                        else
                        {
                            // Something weird happened
                            myStateMachine.jumpState(VCS_DISCONNECT);
                            myStateMachine.queueState(VCS_NO_CONNECTION);
                        }

                        if (netEvent.packet) enet_packet_destroy(netEvent.packet);

                        // Move to the next state
                        break;
                    }
                }
            }

            if (createCharacter.wasPushed())
            {
                // Create a new character
                myStateMachine.jumpState(VCS_CREATE_CHARACTER);
                break;
            }

            if (deleteCharacter.wasPushed())
            {
                unsigned int selection = characterList.getSelection();
                if (selection < numCharacterIDs)
                {
                    enet_peer_send(myENetServer, 0, ::Evidyon::Network::Packet::Client::Encode::deleteCharacter(characterIDs[selection], ""));
                    bool deleted = false;
                    unsigned long startTime = dcxWin32Clock::getSystemTime();
                    while(dcxWin32Clock::getSystemTime() - startTime < 1000 &&
                         !(deleted = (enet_host_service(myENetClient, &netEvent, 1000) > 0 && netEvent.type == ENET_EVENT_TYPE_RECEIVE && netEvent.channelID == 0)));
                    if (deleted)
                    {
                        ::Evidyon::Network::Packet::Message msg = ::Evidyon::Network::Packet::NETMSG_INVALID;
                        ::Evidyon::Network::Packet::decodeMessageType(netEvent.packet, &msg);
                        deleted = msg == ::Evidyon::Network::Packet::NETMSG_REQUESTACTIONRESULT;
                    }

                    myStateMachine.jumpState(VCS_LOGGED_INTO_ACCOUNT); // this is cheating
                    break;
                }
            }

            //if (buttonClicked == BUTTON_LOGOUT)
            if (logout.wasPushed())
            {
                logout.resetPushed();
                {
                    // Send the log out message
                    // TODO: this should be a state of its own, since the client needs to be able to log out of
                    // the world directly!
                    bool loggedOut;
                    {
                        enet_peer_send(myENetServer, 0, ::Evidyon::Network::Packet::Client::Encode::logoutAccount());
                        loggedOut = false;
                        unsigned long startTime = dcxWin32Clock::getSystemTime();
                        while(dcxWin32Clock::getSystemTime() - startTime < 1000 &&
                             !(loggedOut = (enet_host_service(myENetClient, &netEvent, 1000) > 0 && netEvent.type == ENET_EVENT_TYPE_RECEIVE && netEvent.channelID == 0)));
                        if (loggedOut)
                        {
                            ::Evidyon::Network::Packet::Message msg = ::Evidyon::Network::Packet::NETMSG_INVALID;
                            ::Evidyon::Network::Packet::decodeMessageType(netEvent.packet, &msg);
                            loggedOut = msg == ::Evidyon::Network::Packet::NETMSG_LOGOUTACCOUNT_CONFIRMED;
                        }
                        if (loggedOut)
                        {
                            loggedOut = !APP_WARNING(!Decode::logoutAccountConfirmed(netEvent.packet))
                                                    ("Failed to decode logoutAccountConfirmed");
                        }
                        enet_packet_destroy(netEvent.packet);
                    }

                    // If we logged out successfully, that means we're still connected and can go to
                    // the main menu state; if not, the connection has likely been dropped
                    if (loggedOut)
                    {
                        myStateMachine.jumpState(VCS_ACCOUNT_LOGIN);
                    }
                    else
                    {
                        myStateMachine.jumpState(VCS_DISCONNECT);
                        myStateMachine.queueState(VCS_NO_CONNECTION);
                    }

                } break;
            }
        }

        // If the user presses space + escape, they're trying to exit
        if (myKeyboardInputSource.isKeyDown(GUIKEY_ESCAPE) &&
            myKeyboardInputSource.isKeyDown(GUIKEY_SPACE))
            break;

        if (characterList.selectionChanged()) {
            unsigned int selection;
            if ((selection = characterList.getSelection()) >= MAX_AVATARS_PER_ACCOUNT) {
                enterWorld.unregisterElements(&gui_primary_layer_);
                enterWorld.getMouseSentinel()->removeFromMouseSentinelChain();
                deleteCharacter.unregisterElements(&gui_primary_layer_);
                deleteCharacter.getMouseSentinel()->removeFromMouseSentinelChain();
                lastSelectionWasValid = false;
            }
            else {

                current_x = character_info[selection].world_x;
                current_y = character_info[selection].world_y;

                // Add the elements, if necessary
                if (!lastSelectionWasValid)
                {
                    enterWorld.registerElements(&gui_primary_layer_);
                    enterWorld.getMouseSentinel()->setParentMouseSentinel(myMouseInputSource.getPrimarySentinel());
                    deleteCharacter.registerElements(&gui_primary_layer_);
                    deleteCharacter.getMouseSentinel()->setParentMouseSentinel(myMouseInputSource.getPrimarySentinel());
                }

                lastSelectionWasValid = true;

                //global_map_manager_.update(current_x, current_y);

                global_actor_manager_.reset();
/*
                // Find actor
                const int actorID = 0;
                global_actor_manager_.acquire(0, &internalActor);
                CONFIRM(internalActor) else return;

                internalActor->reset(current_x, current_y);
                global_actor_manager_.setActorDescription(actorID, character_info[selection].actor_type);*/
            }
        }

        // Rotate the character
        /*if (internalActor)
        {
            internalActor->lastUpdate.direction = currentTime;
            internalActor->renderAngle = currentTime;
        }*/

        // Draw the scene
        if (allowFrameToBeRendered(dcxWin32Clock::getAccurateSystemTime()))
        {
            float timeSinceLastRenderedFrame = lastRenderedFrameTime - currentTime;
            lastRenderedFrameTime = currentTime;

            // Clear the backbuffer and the zbuffer
            myD3DDevice->Clear(0, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER,
                               /*D3DCOLOR_XRGB(93, 109, 82)*/
                               D3DCOLOR_XRGB(0, 0, 0), 1.0f, 0);

            //global_actor_manager_.update(currentTime, timeSinceLastRenderedFrame);

            // Begin the scene
            if (SUCCEEDED(myD3DDevice->BeginScene())) {

              //myIsometricCamera.setPosition(current_x, current_y);
              //myIsometricCamera.rebuild(myD3DParams.BackBufferWidth, myD3DParams.BackBufferHeight, 0.40f, 0);
              //myIsometricCamera.initialize3DRendering(myD3DDevice);

              //// Set up the camera
              //myD3DDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
              //myD3DDevice->SetRenderState(D3DRS_LIGHTING, TRUE);

              //myD3DDevice->SetRenderState(D3DRS_AMBIENT,D3DCOLOR_XRGB(128,128,128));

              //// Draw the 3D scene
              //for (unsigned int i = 0; i < global_rendering_manager_.numberOfTextures(); ++i) {
              //  CONFIRM(mySceneryRenderer.renderTextureSubset(&myIsometricCamera, i, &global_rendering_manager_));
              //  CONFIRM(global_map_manager_.render(&myIsometricCamera, i));
              //  CONFIRM(global_actor_manager_.render(myD3DDevice, i, &myIsometricCamera, &global_rendering_manager_));
              //}

              //myD3DDevice->SetRenderState(D3DRS_FOGENABLE, FALSE);
              //myD3DDevice->SetRenderState(D3DRS_LIGHTING, FALSE);

              renderLoginBackground();

              // Draw the (new) GUI
              global_rendering_manager_.resetCurrentTexture();
              myD3DDevice->Clear(0, NULL, D3DCLEAR_ZBUFFER, 0, 1.0f, 0);
              gui_primary_layer_.render();
              gui_primary_layer_.flush();

              // End the scene
              myD3DDevice->EndScene();
            }

            // Present the backbuffer contents to the display
            CONFIRM(SUCCEEDED(myD3DDevice->Present(NULL, NULL, NULL, NULL))) else {
              myStateMachine.branchState(VCS_LOST_D3DDEVICE);
              break;
            }
        }
    }

    logout.unregisterElements(&gui_primary_layer_);

    logout.getMouseSentinel()->removeFromMouseSentinelChain();
    enterWorld.unregisterElements(&gui_primary_layer_);

    enterWorld.getMouseSentinel()->removeFromMouseSentinelChain();
    deleteCharacter.unregisterElements(&gui_primary_layer_);

    deleteCharacter.getMouseSentinel()->removeFromMouseSentinelChain();
    createCharacter.unregisterElements(&gui_primary_layer_);

    createCharacter.getMouseSentinel()->removeFromMouseSentinelChain();
    characterList.destroy(&gui_primary_layer_);
}