//#include "stdafx.h"
//#include "../shared/evidyon_version.h"
//#include "volucrisguidefs.h"
//
//
//
//namespace SavedAccountID {
//static const char* ACCOUNT_ID_FILE = "paif.dat";
//void store(unsigned int account_id) {
//  FILE* pf;
//  if (0 == fopen_s(&pf, ACCOUNT_ID_FILE, "wb")) {
//    fwrite(&account_id, sizeof(account_id), 1, pf);
//    fclose(pf);
//  }
//}
//std::string retrieve(bool* success) {
//  FILE* pf;
//  if (0 == fopen_s(&pf, ACCOUNT_ID_FILE, "rb")) {
//    unsigned int account_id;
//    fread(&account_id, sizeof(account_id), 1, pf);
//    fclose(pf);
//    char account_str[16];
//    sprintf_s(account_str, sizeof(account_str), "%lu", account_id);
//    *success = true;
//    return std::string(account_str);
//  }
//  else {
//    *success = false;
//    return "";
//  }
//}
//}
//
//
//class VolucrisGUITabSequenceGUIEditableTextLine : public VolucrisGUIEditableTextLine
//{
//    class TabKeyboardSentinel : public GUIKeyboardSentinel
//    {
//        public:
//
//            /**
//             * 
//             */
//            TabKeyboardSentinel()
//            {
//                myReturnButton = 0;
//                myNextSentinel = 0;
//            }
//
//            /**
//             * 
//             */
//            void setNext(GUIKeyboardSentinel* sentinel, VolucrisGUIButton* returnButton)
//            {
//                myNextSentinel = sentinel;
//                myReturnButton = returnButton;
//            }
//
//            /**
//             * When a key is released, this method is invoked by the keyboard input source
//             * @return 'true' To indicate the event should stop propogating, 'false' otherwise
//             */
//            bool onKeyUp(GUIKey key, GUIKeyboardInputSource* keyboard)
//            {
//                if (key == GUIKEY_RETURN)
//                {
//                    if (myReturnButton)
//                        myReturnButton->push();
//                    else
//                        key = GUIKEY_TAB;
//                }
//
//                if (key == GUIKEY_TAB)
//                {
//                    // Change the focus
//                    if (myNextSentinel)
//                        myNextSentinel->obtainKeyboardFocus();
//
//                    // Don't propogate this message
//                    return true;
//                }
//
//                return false;
//            }
//
//        protected:
//
//            /// If a return is obtained, it will push this button if it is valid.  Otherwise,
//            /// 'return' acts like a tab.
//            VolucrisGUIButton* myReturnButton;
//
//            GUIKeyboardSentinel* myNextSentinel;
//    };
//
//    public:
//
//        VolucrisGUITabSequenceGUIEditableTextLine()
//        {
//            myTabKeyboardSentinel.setParentKeyboardSentinel(getKeyboardSentinel());
//        }
//
//        void setNextInChain(VolucrisGUITabSequenceGUIEditableTextLine* tabElement, VolucrisGUIButton* returnButton)
//        {
//            myTabKeyboardSentinel.setNext(tabElement->getKeyboardSentinel(), returnButton);
//        }
//
//
//    protected:
//
//        TabKeyboardSentinel myTabKeyboardSentinel;
//};
//
//
//
//
//
////------------------------------------------------------------------------------------------------
//// Name:  stateAccountLogin
//// Desc:  
////------------------------------------------------------------------------------------------------
//void VolucrisClient::stateAccountLogin()
//{
//using namespace ::Evidyon::Network::Packet;
//using namespace ::Evidyon::Network::Packet::Client;
//
//    // Clear the screen
//
//    GUICanvas titleCanvas;
//
//    titleCanvas.setSourceCanvas(gui_primary_layer_.getPrimaryCanvas());
//    titleCanvas.alignXLeft(200);
//    titleCanvas.alignYTop(100);
//    titleCanvas.absoluteDepth(0.01f);
//    titleCanvas.absoluteWidth(512);
//    titleCanvas.absoluteHeight(128);
//
//    GUICanvas titleCanvasLeft, titleCanvasRight, titleCanvasRight2;
//    titleCanvasLeft.setSourceCanvas(gui_primary_layer_.getPrimaryCanvas());
//    titleCanvasLeft.alignXLeft(200-512);
//    titleCanvasLeft.alignYTop(100);
//    titleCanvasLeft.absoluteDepth(0.01f);
//    titleCanvasLeft.absoluteWidth(512);
//    titleCanvasLeft.absoluteHeight(128);
//    //titleCanvasLeft.realign();
//    titleCanvasRight.setSourceCanvas(gui_primary_layer_.getPrimaryCanvas());
//    titleCanvasRight.alignXLeft(200 + 512);
//    titleCanvasRight.alignYTop(100);
//    titleCanvasRight.absoluteDepth(0.01f);
//    titleCanvasRight.absoluteWidth(512);
//    titleCanvasRight.absoluteHeight(128);
//    //titleCanvasRight.realign();
//    titleCanvasRight2.alignXLeft(200 + 512 + 512);
//    titleCanvasRight2.alignYTop(100);
//    titleCanvasRight2.absoluteDepth(0.01f);
//    titleCanvasRight2.absoluteWidth(512);
//    titleCanvasRight2.absoluteHeight(128);
//    //titleCanvasRight2.realign();
//
//    const int MAIN_TITLE_TEXTURE = 50;
//    const int BACKGROUND_TITLE_TEXTURE = 51;
//
//    gui_primary_layer_.setTexture(MAIN_TITLE_TEXTURE, myResources.images.d3dTextureTitle);
//    gui_primary_layer_.setTexture(BACKGROUND_TITLE_TEXTURE, myResources.images.d3dTextureTitleBackground);
//
//    GUIPictureElement title;
//    title.setTexture(MAIN_TITLE_TEXTURE);
//    title.setSourceRegion(NULL, &gui_primary_layer_);
//    title.setSourceCanvas(&titleCanvas);
//    gui_primary_layer_.registerElement(&title);
//
//    GUIPictureElement titleLeft, titleRight, titleRight2;
//    titleLeft.setTexture(BACKGROUND_TITLE_TEXTURE);
//    titleLeft.setSourceRegion(NULL, &gui_primary_layer_);
//    titleLeft.setSourceCanvas(&titleCanvasLeft);
//    gui_primary_layer_.registerElement(&titleLeft);
//    titleRight.setTexture(BACKGROUND_TITLE_TEXTURE);
//    titleRight.setSourceRegion(NULL, &gui_primary_layer_);
//    titleRight.setSourceCanvas(&titleCanvasRight);
//    gui_primary_layer_.registerElement(&titleRight);
//    titleRight2.setTexture(BACKGROUND_TITLE_TEXTURE);
//    titleRight2.setSourceRegion(NULL, &gui_primary_layer_);
//    titleRight2.setSourceCanvas(&titleCanvasRight2);
//    gui_primary_layer_.registerElement(&titleRight2);
//
//    VolucrisGUITitledPanel myLoginPanel;
//    myLoginPanel.setFont(&myWindowFont);
//    myLoginPanel.setText("Log Into Account");
//    myLoginPanel.getCanvas()->setSourceCanvas(gui_primary_layer_.getPrimaryCanvas());
//    myLoginPanel.getCanvas()->alignXCenter(0);
//    myLoginPanel.getCanvas()->alignYCenter(150);
//    myLoginPanel.getCanvas()->absoluteWidth(720);
//    myLoginPanel.getCanvas()->absoluteHeight(250);
//    myLoginPanel.getCanvas()->absoluteDepth(0.5f);
//    myLoginPanel.registerElements(&gui_primary_layer_);
//
//
//    enum StateCreateAccountButtons {
//        GUIBUTTON_CREATEACCOUNT,
//        GUIBUTTON_CANCEL,
//    };
//
//
//    const char* newButtonText[] = { "Create Account", "Cancel" };
//
//    VolucrisGUIButton newButtons[2];
//    for (int i = 0; i < 2; ++i)
//    {
//        newButtons[i].getMouseSentinel()->setParentMouseSentinel(myMouseInputSource.getPrimarySentinel());
//        newButtons[i].setText(newButtonText[i]);
//        newButtons[i].setFont(&myWindowFont);
//        newButtons[i].getCanvas()->setSourceCanvas(myLoginPanel.getCanvas());
//        newButtons[i].getCanvas()->alignXRight(-15);
//        newButtons[i].getCanvas()->alignYTop(130 + 35 * i);
//        newButtons[i].getCanvas()->absoluteWidth(250);
//        newButtons[i].getCanvas()->absoluteHeight(32);
//        newButtons[i].getCanvas()->absoluteDepth(0.35f);
//        //newButtons[i].getCanvas()->realign();
//
//        newButtons[i].registerElements(&gui_primary_layer_);
//    }
//
//    GUICanvas accountNumberLabelCanvas, passwordLabelCanvas;
//    GUITextLabelElement accountNumberLabel, passwordLabel;
//    accountNumberLabel.setFont(&myWindowFont);
//    accountNumberLabel.setSourceCanvas(&accountNumberLabelCanvas);
//    accountNumberLabel.setText("Account:");
//    accountNumberLabel.setTextColor(VolucrisGUIDefs::CONTENT_COMPONENT_TEXT_COLOR);
//    passwordLabel.setFont(&myWindowFont);
//    passwordLabel.setSourceCanvas(&passwordLabelCanvas);
//    passwordLabel.setText("Password:");
//    passwordLabel.setTextColor(VolucrisGUIDefs::TEXT_INPUT_COLOR);
//
//    accountNumberLabelCanvas.relativeDepth(-0.01f);
//    accountNumberLabelCanvas.alignXLeft(+100);
//    accountNumberLabelCanvas.alignYTop(+25);
//    accountNumberLabelCanvas.absoluteHeight(30);
//    accountNumberLabelCanvas.absoluteWidth(0);
//    accountNumberLabelCanvas.setSourceCanvas(myLoginPanel.getCanvas());
//    //accountNumberLabelCanvas.realign();
//
//    passwordLabelCanvas.relativeDepth(-0.01f);
//    passwordLabelCanvas.alignXLeft(+125);
//    passwordLabelCanvas.alignYTop(+60);
//    passwordLabelCanvas.absoluteHeight(30);
//    passwordLabelCanvas.absoluteWidth(0);
//    passwordLabelCanvas.setSourceCanvas(myLoginPanel.getCanvas());
//    //passwordLabelCanvas.realign();
//    gui_primary_layer_.registerElement(&accountNumberLabel);
//    gui_primary_layer_.registerElement(&passwordLabel);
//
//    GUICanvas notificationCanvas;
//    notificationCanvas.relativeDepth(-0.01f);
//    notificationCanvas.alignXLeft(20);
//    notificationCanvas.alignYBottom(-10);
//    notificationCanvas.absoluteWidth(300);
//    notificationCanvas.absoluteHeight(130);
//    notificationCanvas.setSourceCanvas(myLoginPanel.getCanvas());
//    GUITextLabelElement notification;
//    notification.setFont(&myWindowFont);
//    notification.setSourceCanvas(&notificationCanvas);
//    notification.setTextColor(VolucrisGUIDefs::CONTENT_COMPONENT_TEXT_COLOR);
//    notification.setText("");
//    notification.setAlignment(GUIALIGN_LEFT);
//    gui_primary_layer_.registerElement(&notification);
//
//
//    VolucrisGUITabSequenceGUIEditableTextLine accountNumberInput;
//    accountNumberInput.configure(&myWindowFont, false, true, VolucrisGUIDefs::TEXT_INPUT_COLOR, 7);
//    accountNumberInput.getCanvas()->setSourceCanvas(myLoginPanel.getCanvas());
//    accountNumberInput.getCanvas()->alignXLeft(200);
//    accountNumberInput.getCanvas()->alignYTop(25);
//    accountNumberInput.getCanvas()->absoluteWidth(200);
//    accountNumberInput.getCanvas()->absoluteHeight(32);
//    accountNumberInput.getCanvas()->absoluteDepth(0.2f);
//    //accountNumberInput.getCanvas()->realign();
//    accountNumberInput.getKeyboardSentinel()->setParentKeyboardSentinel(myKeyboardInputSource.getPrimarySentinel());
//    accountNumberInput.getMouseSentinel()->setParentMouseSentinel(myMouseInputSource.getPrimarySentinel());
//    accountNumberInput.registerElements(&gui_primary_layer_);
//
//    VolucrisGUITabSequenceGUIEditableTextLine passwordInput;
//    passwordInput.configure(&myWindowFont, true, false, VolucrisGUIDefs::TEXT_INPUT_COLOR, MAX_PASSWORD_LENGTH);
//    passwordInput.getCanvas()->setSourceCanvas(myLoginPanel.getCanvas());
//    passwordInput.getCanvas()->alignXLeft(200);
//    passwordInput.getCanvas()->alignYTop(60);
//    passwordInput.getCanvas()->absoluteWidth(200);
//    passwordInput.getCanvas()->absoluteHeight(32);
//    passwordInput.getCanvas()->absoluteDepth(0.2f);
//    //passwordInput.getCanvas()->realign();
//    passwordInput.getKeyboardSentinel()->setParentKeyboardSentinel(myKeyboardInputSource.getPrimarySentinel());
//    passwordInput.getMouseSentinel()->setParentMouseSentinel(myMouseInputSource.getPrimarySentinel());
//    passwordInput.registerElements(&gui_primary_layer_);
//
//    passwordInput.setNextInChain(&accountNumberInput, &newButtons[GUIBUTTON_ENTERWORLD]);
//    accountNumberInput.setNextInChain(&passwordInput, NULL);
//    accountNumberInput.obtainFocus();
//
//
//    // By default, exit if the loop breaks
//    myStateMachine.queueState(VCS_DISCONNECT);
//    myStateMachine.queueState(VCS_SHUTDOWN);
//
//    // Tell the server our version
//    enet_peer_send(myENetServer, 0, ::Evidyon::Network::Packet::Client::Encode::tellClientVersionInfo(myResources.fileVersion, VERSION));
//
//    enum LastRequestedAction
//    {
//        LAR_INVALID,
//        LAR_CREATEACCOUNT,
//        LAR_ENTERWORLD,
//    };
//    LastRequestedAction lastRequestedAction = LAR_INVALID;
//
//    { // Get the player's account number and switch focus to the password if it was found
//      bool succeeded;
//      accountNumberInput.setText(SavedAccountID::retrieve(&succeeded));
//      if (succeeded) {
//        passwordInput.obtainFocus();
//      }
//    }
//
////#if defined(_DEBUG)
////    accountNumberInput.setText("1435526");
////    passwordInput.setText("kwg88ca");
////#endif
//#if defined(_DEBUG)
//    accountNumberInput.setText("1892795");
//    passwordInput.setText("fewa");
//#endif
//
//    // Loop until we get a reply
//    while(windowsMessagePump())
//    {
//        // Poll the connection to keep it alive
//        {
//            ENetEvent netEvent;
//            if (enet_host_service(myENetClient, &netEvent, 0) > 0)
//            {
//                if (netEvent.type == ENET_EVENT_TYPE_DISCONNECT)
//                {
//                    myStateMachine.jumpState(VCS_CONNECT);
//                    break;
//                }
//                else if (netEvent.type == ENET_EVENT_TYPE_RECEIVE && netEvent.channelID == 0)
//                {
//                    ::Evidyon::Network::Packet::Message message;
//                    ::Evidyon::Network::Packet::decodeMessageType(netEvent.packet, &message);
//                    if (message == ::Evidyon::Network::Packet::NETMSG_TELLSERVERVERSIONUPDATEINFO)
//                    {
//                        bool versionIsValid, needsNewGamefile, needsNewClient;
//                        std::string updateText;
//                        CONFIRM(Decode::tellServerVersionUpdateInfo(netEvent.packet, &versionIsValid, &needsNewGamefile, &needsNewClient, &updateText)) else continue;
//
//                        // Set the text
//                        if (!versionIsValid)
//                        {
//                            std::string newUpdateText = "\n\nYour version is outdated;\n";
//                            if (needsNewGamefile) newUpdateText += "download the new installer\n\n";
//                            else if (needsNewClient) newUpdateText += "get a new client at Evidyon.com\n\n";
//                            updateText = updateText + newUpdateText;
//                        }
//
//                        // Set in the panel
//                        notification.setText(updateText);
//                    }
//                    else if (message == ::Evidyon::Network::Packet::NETMSG_REQUESTACTIONRESULT)
//                    {
//                        ::Evidyon::Network::Packet::RequestActionResult rar;
//                        CONFIRM(lastRequestedAction == LAR_ENTERWORLD) else continue;
//                        CONFIRM(Decode::requestActionResult(netEvent.packet, &rar)) else continue;
//
//                        if (rar == RAR_FAILED_ACCOUNTDOESNTEXIST)
//                        {
//                            notification.setText("Account doesn't exist");
//                        }
//                        else if (rar == RAR_FAILED_INCORRECTPASSWORD)
//                        {
//                            notification.setText("Password incorrect");
//                        }
//                        else if (rar == RAR_FAILED_ALREADYACTIVE)
//                        {
//                            notification.setText("Account is already active");
//                        }
//                        else if (rar == RAR_FAILED_VERSIONOUTOFDATE)
//                        {
//                            notification.setText("Your client is outdated;\ngo to evidyon.com to\ndownload a new one");
//                        }
//                        else if (rar == RAR_SUCCEEDED)
//                        {
//                            // The client logged in
//                            myStateMachine.jumpState(VCS_LOGGED_INTO_ACCOUNT);
//                            enet_packet_destroy(netEvent.packet);
//                            break;
//                        }
//                        else
//                        {
//                            DEBUG_WARNING("Unrecognized request action result");
//                            notification.setText("Error logging in.");
//                        }
//                    }
//                    else if (message == ::Evidyon::Network::Packet::NETMSG_CREATEACCOUNT_SUCCEEDED)
//                    {
//                        CONFIRM(lastRequestedAction == LAR_CREATEACCOUNT) else continue;
//                        unsigned int accountID;
//                        CONFIRM(Decode::createAccountSucceeded(netEvent.packet, &accountID)) else continue;
//
//                        char accountText[512];
//                        sprintf_s(accountText, sizeof(accountText), "%lu", accountID);
//                        accountNumberInput.setText(accountText);
//                        passwordInput.setText("");
//                        notification.setText("Account created successfully");
//                        SavedAccountID::store(accountID);
//                    }
//                }
//
//                // Free packet memory
//                enet_packet_destroy(netEvent.packet);
//            }
//        }
//
//        // Exit the program
//        if (newButtons[GUIBUTTON_QUIT].wasPushed())
//            break;
//
//        if (newButtons[GUIBUTTON_CREATEACCOUNT].wasPushed())
//        {
//            newButtons[GUIBUTTON_CREATEACCOUNT].resetPushed();
//
//            const char* password = passwordInput.getText();
//            if (!password || password[0] == '\0')
//            {
//                notification.setText("Please type a valid password\n"\
//                                      "before creating an account.\n"\
//                                      "The account number will be filled\n"\
//                                      "for you.");
//                continue;
//            }
//
//            // Contact the server and try to make the account
//            enet_peer_send(myENetServer, 0, ::Evidyon::Network::Packet::Client::Encode::createAccount(passwordInput.getText()));
//            lastRequestedAction = LAR_CREATEACCOUNT;
//            notification.setText("Creating account");
//
//            newButtons[GUIBUTTON_CREATEACCOUNT].resetPushed();
//        }
//
//        if (newButtons[GUIBUTTON_ENTERWORLD].wasPushed())
//        {
//            newButtons[GUIBUTTON_ENTERWORLD].resetPushed();
//
//            // Whether or not we logged in successfully
//            bool loggedIntoAccount = false;
//
//            // Get the account ID from the text box
//            //unsigned int accountID = (unsigned int)atol(accountText);
//            unsigned int accountID = (unsigned int)atol(accountNumberInput.getText());
//            SavedAccountID::store(accountID);
//
//            // If we were able to obtain an account ID, access the account
//            if (accountID != 0)
//            {
//                // Send an access message
//                enet_peer_send(myENetServer, 0, ::Evidyon::Network::Packet::Client::Encode::accessAccount(accountID, passwordInput.getText()));
//                lastRequestedAction = LAR_ENTERWORLD;
//
//                notification.setText("Logging into account");
//            }
//            else
//                notification.setText("Please enter an account number\nor create an account first");
//        }
//
//        if (allowFrameToBeRendered(dcxWin32Clock::getAccurateSystemTime()))
//        {
//            // Clear the backbuffer and the zbuffer
//            myD3DDevice->Clear(0, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(0,0,0), 1.0f, 0);
//
//            // Begin the scene
//            if (SUCCEEDED(myD3DDevice->BeginScene()))
//            {
//                myD3DDevice->Clear(0, NULL, D3DCLEAR_ZBUFFER, 0, 1.0f, 0);
//
//                renderLoginBackground();
//
//                // Draw the (new) GUI
//                gui_primary_layer_.render();
//
//                // End the scene
//                myD3DDevice->EndScene();
//            }
//
//            // Present the backbuffer contents to the display
//            CONFIRM(SUCCEEDED(myD3DDevice->Present(NULL, NULL, NULL, NULL))) else
//            {
//                // Wait for the device to come back, then return here
//                myStateMachine.branchState(VCS_LOST_D3DDEVICE);
//                break;
//            }
//        }
//
//        // If the user presses space + escape, they're trying to exit
//        if (myKeyboardInputSource.isKeyDown(GUIKEY_ESCAPE) &&
//            myKeyboardInputSource.isKeyDown(GUIKEY_SPACE))
//            break;
//    }
//
//
//    for (int i = 0; i < 3; ++i) {
//      newButtons[i].unregisterElements(&gui_primary_layer_);
//    }
//    myLoginPanel.unregisterElements(&gui_primary_layer_);
//    gui_primary_layer_.unregisterElement(&titleRight2);
//    gui_primary_layer_.unregisterElement(&titleRight);
//    gui_primary_layer_.unregisterElement(&titleLeft);
//    gui_primary_layer_.unregisterElement(&title);
//    gui_primary_layer_.unregisterElement(&accountNumberLabel);
//    gui_primary_layer_.unregisterElement(&passwordLabel);
//    gui_primary_layer_.unregisterElement(&notification);
//
//    gui_primary_layer_.removeTexture(MAIN_TITLE_TEXTURE);
//    gui_primary_layer_.removeTexture(BACKGROUND_TITLE_TEXTURE);
//
//    passwordInput.unregisterElements(&gui_primary_layer_);
//    accountNumberInput.unregisterElements(&gui_primary_layer_);
//}