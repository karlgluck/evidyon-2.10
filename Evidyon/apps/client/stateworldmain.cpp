//------------------------------------------------------------------------------------------------
// File:    stateworldmain.cpp
//
// Desc:    
//
// Copyright (c) 2007-2008 Unseen Studios.   All rights reserved.
//------------------------------------------------------------------------------------------------
#include "stdafx.h"
#include "worldinteractionmanager.h"
#include <dc/filestream>
#include "../shared/evidyon_version.h"
#include "volucrisguidefs.h"
#include "../common/gui/guicanvasmatrix.h"
#include "volucrisguibuttonpanel.h"
#include "volucrisguiglobalchatdisplay.h"
#include "../shared/evidyon_world.h"
#include "../shared/client-server/actorsync.h"
#include "./packets/tellactorfulldescriptions-decoder.h"
#include "../shared/evidyon_specialfx.h"
#include "../shared/client-server/fastsync.h"
#include "../shared/evidyon_network.h"
#include "evidyonguienchantmenticons.h"


#define STAT_BAR_WIDTH  200
#define DOUBLE_MAX  1e+37

class VolucrisPartyDisplay;



#include <dcx/bitmask>
#include "../common/gui/guiscrollablelist.h"




#include "evidyonguigameplaycontrols.h"


namespace Evidyon {

class EvidyonGUIModule_KeyBindingList : public GUIModule {

  GUIFont* button_font_;
  EvidyonGUIScrollableButtonList spell_list_;
  const ClientSpellDescription* spell_descriptions_;
  size_t number_of_spell_descriptions_;

};



}





//------------------------------------------------------------------------------------------------
// Name:  stateWorldMain
// Desc:  
//------------------------------------------------------------------------------------------------
void VolucrisClient::stateWorldMain() {
using namespace ::Evidyon::Network::Packet;
using namespace ::Evidyon::Network::Packet::Client;
    SetCursor(NULL);
    ShowCursor(FALSE);

    // DONT reset the actor manager, otherwise the client's actor disappears!
    //global_actor_manager_.reset();

    // Clear the screen
    myItemsOnMapTable.clear();
    mySceneryRenderer.clear();
    if (myUsingMixedDeviceFlag)
        CONFIRM(myD3DDevice->SetSoftwareVertexProcessing(TRUE));

    myMouseInputSource.setMouseLock(true);

    // Build the camera
    myIsometricCamera.rebuildXenimusCamera(myD3DParams.BackBufferWidth, myD3DParams.BackBufferHeight);

    GUIFont font(VolucrisGUIDefs::WINDOW_FONT_TEXTURE_INDEX);
    if (APP_ERROR(!font.create(myD3DDevice, "Trebuchet MS", 12))("Couldn't create font")) return;

    gui_primary_layer_.setTexture(font.getTextureID(), font.getTexture());

    GUIFont speechFont(VolucrisGUIDefs::TITLE_FONT_TEXTURE_INDEX);
    { // Init the speech font from the bitmap
      DWORD character_indices[128-32];
      character_indices[0] = 8*11+6; // ' '
      for (int i = 33; i < 128; ++i) {
        character_indices[i-32] = i-33;
      }
      speechFont.create(myResources.images.d3dTextureFont, 16, 16, character_indices, 1.2f, 0);
    }
    gui_primary_layer_.setTexture(speechFont.getTextureID(), speechFont.getTexture());

    EvidyonGUITooltipBox tooltip_box;
    {
      GUISize sz = { 180, 85 };
      tooltip_box.setContentSize(sz);
    }
    tooltip_box.create(&gui_primary_layer_, 40, &mouse_cursor_canvas_);
    tooltip_box.becomeOwner(this);
    tooltip_box.update(dcxWin32Clock::getAccurateSystemTime());


    //EvidyonGUIGameDialogMenu menu;
    //menu.setFont(&font);
    //menu.setParentSentinels(myMouseInputSource.getPrimarySentinel(),
    //                        myKeyboardInputSource.getPrimarySentinel());
    //menu.getCanvas()->setSourceCanvas(gui_primary_layer_.getPrimaryCanvas());
    //menu.getCanvas()->alignXCenter(0);
    //menu.getCanvas()->alignYTop(16);
    //menu.getCanvas()->absoluteDepth(0.2f);
    //menu.registerElements(&gui_primary_layer_);

    Evidyon::EvidyonGUIStatusBars status_bars;
    status_bars.init(myD3DParams.BackBufferWidth, myD3DParams.BackBufferHeight);
    status_bars.registerElements(&gui_primary_layer_);

    EvidyonGUIEnchantmentIcons enchantment_icons;
    enchantment_icons.init(gui_primary_layer_.getPrimaryCanvas(), status_bars.getSize());
    enchantment_icons.registerElements(&gui_primary_layer_);

    // todo: put the storage somewhere else
    ItemInStorage storage[Evidyon::MAX_ITEMS_IN_STORAGE];
    memset(storage, 0, sizeof(storage));

    EvidyonGUIGameplayControls gameplay_controls_;
    {
      char keymapName[MAX_AVATAR_NAME_LENGTH + 16];
      sprintf_s(keymapName, sizeof(keymapName), "%s.keys", myCharacter.name);
      gameplay_controls_.create(keymapName,
                                &font,
                                &gui_primary_layer_,
                                myMouseInputSource.getPrimarySentinel(),
                                myKeyboardInputSource.getPrimarySentinel(),
                                &mouse_cursor_canvas_,
                                myResources.items.itemDescriptionArray,
                                myResources.items.itemDescriptionArraySize,
                                myCharacter.inventory,
                                myResources.spells.spell_descriptions_,
                                myResources.spells.number_of_spell_descriptions,
                                &myCharacter.effectAvailability,
                                status_bars.getSize(),
                                &myIsometricCamera,
                                &global_actor_manager_,
                                storage);
    }

    LPDIRECT3DTEXTURE9 guiTexture = NULL;

    //BazaarDisplay bazaar_display;
    ////bazaar_display.create(myResources.items.itemDescriptionArray, myResources.items.itemDescriptionArraySize,
    ////                      &font, myMouseInputSource.getPrimarySentinel(), &world_keyboard_input_sentinel,
    ////                      &user_action_queue, &gui_primary_layer_);
    ////bazaar_display.updateMoney(myCharacter.money);

    VolucrisGUIChatLog chat_log;
    chat_log.create(&font, myMouseInputSource.getPrimarySentinel());
    chat_log.getBackgroundCanvas()->setSourceCanvas(gui_primary_layer_.getPrimaryCanvas());
    chat_log.getBackgroundCanvas()->alignXCenter(0);
    chat_log.getBackgroundCanvas()->alignYTop(+10);
    chat_log.getBackgroundCanvas()->relativeWidth(-20);
    chat_log.getBackgroundCanvas()->absoluteHeight(220);
    chat_log.getBackgroundCanvas()->absoluteDepth(0.5f);

    VolucrisGUIGlobalChatDisplay chat_display;
    chat_display.create(&font, guiTexture, gui_primary_layer_.getPrimaryCanvas());
    chat_display.show(&gui_primary_layer_);


    // this component is used to display players' names
    GUITextLabelElement mouse_over_name_display;
    GUICanvas mouse_over_name_display_canvas;
    {
      mouse_over_name_display_canvas.setSourceCanvas(gui_primary_layer_.getPrimaryCanvas());
      mouse_over_name_display_canvas.relativeWidth(0);
      mouse_over_name_display_canvas.absoluteHeight(30);
      mouse_over_name_display_canvas.alignXCenter(0);
      mouse_over_name_display_canvas.alignYBottom(0);
      mouse_over_name_display_canvas.absoluteDepth(0.9f);
      mouse_over_name_display.setSourceCanvas(&mouse_over_name_display_canvas);
      mouse_over_name_display.setFont(&myWindowFont);
      mouse_over_name_display.setAlignment(GUIALIGN_CENTER);
      mouse_over_name_display.setTextColor(VolucrisGUIDefs::WINDOW_TITLE_TEXT_COLOR);
      gui_primary_layer_.registerElement(&mouse_over_name_display);
    }

    bool take_screenshot = false;
    int last_fast_sync_stat = 0;


    dcxWin32Clock clock;
    double this_frame_time = clock.getAccurateSystemTime();
    double last_frame_time = this_frame_time;
    double last_rendered_frame_time = this_frame_time;

    bool changedHPMPEXP = true;
    bool showPlayerNames = true;

LPDIRECT3DTEXTURE9 light_texture = myResources.images.d3dTextureDayNightLightParticle;
if (light_texture) light_texture->AddRef();
//D3DXCreateTextureFromFile(myD3DDevice, "../../../media/images/light-particle.png", &light_texture);

    bool overheadViewMode = false;
    std::string last_spoken_text_line = ""; // for say again

    myStateMachine.jumpState(VCS_DISCONNECT);
    myStateMachine.queueState(VCS_SHUTDOWN);

    double current_fps = 0.0;
    static const double FPS_EXP_WEIGHT = 0.1;

    // set the maximum delay between frames so that stuff doesn't go nuts if
    // there is a spike in the amount of time between two frames (such as
    // when a new map is loaded)
    static const double MAX_FRAME_DELAY = 1.0 / 20.0;


    // Main game loop
    bool runWorldMain = true;
    while(windowsMessagePump() && runWorldMain) {

      // Advance the current frame
      this_frame_time = clock.getAccurateSystemTime();
      double time_since_last_frame = this_frame_time - last_frame_time;
      time_since_last_frame = min(time_since_last_frame, MAX_FRAME_DELAY);

      // Poll the network connection
      ENetEvent netEvent;
      while (enet_host_service(myENetClient, &netEvent, 0) > 0) {
        if (netEvent.type == ENET_EVENT_TYPE_DISCONNECT) {
            myStateMachine.jumpState(VCS_CONNECT);  // Lost connection; try to reconnect
            runWorldMain = false; // Exit to clean up
            break;
        }
        if (netEvent.type != ENET_EVENT_TYPE_RECEIVE) continue;

        using namespace ::Evidyon::Network::Packet;
        Message message;
        decodeMessageType(netEvent.packet, &message);

        if (message == NETMSG_SLOWSYNC) {
          ActorSyncData actor_updates[128];
          unsigned int actors = 0;
          float time_since_last_sync;
          if (Decode::slowSync(netEvent.packet,
                              &time_since_last_sync,
                              &actors,
                               actor_updates,
                               sizeof(actor_updates))) {
            std::set<ActorID> actors_without_names;
            for (unsigned int i = 0; i < actors; ++i) {
              global_actor_manager_.slowSync(time_since_last_sync,
                                             this_frame_time,
                                            &actor_updates[i],
                                            &actors_without_names);
            }
            for (std::set<ActorID>::iterator i = actors_without_names.begin(); i != actors_without_names.end(); ++i) {
              enet_peer_send(myENetServer, 1, Encode::askActorName(*i));
            }
          }

        } else if (message == ::Evidyon::Network::Packet::NETMSG_FASTSYNC) {
          using namespace ::Evidyon::Network::Packet;
          FastSyncPacket packet;
          if (Decode::fastSync(netEvent.packet,
                              &packet)) {
            global_actor_manager_.fastSync(packet.time_since_last_sync,
                                           this_frame_time,
                                           packet.x_packed * Network::FLOAT_UNPACK_MULTIPLIER,
                                           packet.y_packed * Network::FLOAT_UNPACK_MULTIPLIER,
                                           packet.sync_flags);
            last_fast_sync_stat = packet.stat_type_reset ? 0 : last_fast_sync_stat + 1;
            switch (last_fast_sync_stat) {
              case 0:
              case 2: myCharacter.currentHPPercent  = 1.0f / 127.0f * packet.stat_value; break;
              case 1:
              case 3: myCharacter.currentMPPercent  = 1.0f / 127.0f * packet.stat_value; break;
              case 4: myCharacter.currentEXPPercent = 1.0f / 127.0f * packet.stat_value; break;
            }
            changedHPMPEXP = true;
          }
        } else if (message == ::Evidyon::Network::Packet::NETMSG_TELLACTORFULLDESCRIPTIONS) {
          using namespace ::Evidyon::Network::Packet;
          TellActorFullDescriptionsPacketDecoder decoder(netEvent.packet);
          DecodedFullActorDescription description;
          while (decoder.read(&description)) {
            {
              Actor* existing = global_actor_manager_.get(description.data.sync_.id);
              //DEBUG_INFO ("Got actor description for %lu (%s)", description.data.sync_.id, existing == NULL ? NULL : existing->getName());
            }
            Actor* actor = global_actor_manager_.setActorDescription(&description);
            if (actor) {
              if (global_actor_manager_.getClientActor() == actor) {
                bool enchantment_active[Enchantments::NUMBER_OF_ENCHANTMENT_CLASSES];
                special_fx_manager_.setActorEnchantments(this_frame_time,
                                                         description.data.state_.enchantments,
                                                         actor,
                                                         enchantment_active);
                enchantment_icons.update(enchantment_active);
              } else {
                special_fx_manager_.setActorEnchantments(this_frame_time,
                                                         description.data.state_.enchantments,
                                                         actor,
                                                         NULL);
              }

              for (int i = 0; i < ACTORATTACHMENTPOINT_COUNT; ++i) {
                unsigned short scenery_index = description.data.state_.equipped_items[i];
                mySceneryRenderer.removeScenery(
                    actor->getAttachmentPointScenery((ActorAttachmentPoint)i));
                HSCENERY scenery;
                if (scenery_index < myResources.scenery.sceneryArraySize) {
                  mySceneryRenderer.addScenery(NULL,
                                              &myResources.scenery.sceneryArray[scenery_index],
                                              &scenery);
                } else {
                  memset(&scenery, 0, sizeof(scenery));
                }

                actor->setAttachmentPointScenery((ActorAttachmentPoint)i, scenery);
              }
            }
          }
        }
        else if (message == ::Evidyon::Network::Packet::NETMSG_LEAVEWORLD_CONFIRMED) {
            bool leftWorld = !APP_WARNING(!Decode::leaveWorldConfirmed(netEvent.packet))
                                         ("Failed to decode leaveWorldConfirmed");

            if (leftWorld) {
                // Return to main menu
            }
            else
            {
                // Something weird happened; just quit
                myStateMachine.jumpState(VCS_DISCONNECT);
                myStateMachine.queueState(VCS_NO_CONNECTION);
            }

            // Exit the main loop so we can clean up
            runWorldMain = false;

        } else if (message == ::Evidyon::Network::Packet::NETMSG_ERASEACTORFROMWORLD) {
          int actor_id;
          Decode::eraseActorFromWorld(netEvent.packet, &actor_id);
          global_actor_manager_.release(actor_id);
        } else if (message == NETMSG_CREATEMAGICFX) {
          int target_ids[Evidyon::MAX_SPECIALFX_TARGETS];
          int type, src_actor_id, power, number_of_targets;
          float src_x, src_y;
          CONFIRM(Decode::createMagicFX(netEvent.packet,
                                       &type,
                                       &src_actor_id,
                                       &src_x,
                                       &src_y,
                                       &power,
                                       target_ids,
                                       Evidyon::MAX_SPECIALFX_TARGETS,
                                      &number_of_targets)) else continue;

          // Get targets
          Actor* src_actor = global_actor_manager_.get(src_actor_id);
          Actor* targets[Evidyon::MAX_SPECIALFX_TARGETS];
          for (int i = 0; i < Evidyon::MAX_SPECIALFX_TARGETS; ++i) {
            if (i < number_of_targets) {
              targets[i] = global_actor_manager_.get(target_ids[i]);
            } else {
              targets[i] = NULL;
            }
          }

          // Generate this effect
          special_fx_manager_.createMagicFX(this_frame_time,
                                            type,
                                            power,
                                            src_actor,
                                            src_x,
                                            src_y,
                                            targets,
                                            number_of_targets);

        } else if (message == NETMSG_CREATEPROJECTILE) {
          int projectile_id, projectile_type;
          float x, y, dx, dy;
          CONFIRM(Decode::createProjectile(netEvent.packet,
                                          &projectile_id,
                                          &projectile_type,
                                          &x,
                                          &y,
                                          &dx,
                                          &dy)) else continue;
          special_fx_manager_.createProjectileFX(this_frame_time,
                                                 projectile_id,
                                                 projectile_type,
                                                 x,
                                                 y,
                                                 dx,
                                                 dy);

        } else if (message == NETMSG_DESTROYPROJECTILE) {
          int projectile_id;
          CONFIRM(Decode::destroyProjectile(netEvent.packet,
                                           &projectile_id)) else continue;
          special_fx_manager_.destroyProjectileFX(this_frame_time, projectile_id);
        } if (message == ::Evidyon::Network::Packet::NETMSG_UPDATEINVENTORY ||
              message == ::Evidyon::Network::Packet::NETMSG_TELLINVENTORY) {
          Decode::updateOrTellInventory(netEvent.packet,
                                        myCharacter.inventory,
                                       &myCharacter.highest_inventory_update_index_,
                                        myCharacter.inventory_update_indices_);
          gameplay_controls_.syncInventory();

          // force the dialogs to sync with the mouse
          myMouseInputSource.generateMovementUpdate();
        }
        else if (message == ::Evidyon::Network::Packet::NETMSG_ACTORSPEAKS)
        {
          char speechBuffer[MAX_SPOKEN_TEXT_CHARACTERS+1];
          int actorID;
          char source_name_buffer[256];
          CONFIRM(Decode::actorSpeaks(netEvent.packet,
                                           &actorID,
                                           source_name_buffer, sizeof(source_name_buffer),
                                           speechBuffer, MAX_SPOKEN_TEXT_CHARACTERS+1));

          // Find this actor in the table
          if (actorID == -1) {
            chat_log.addReceivedChatLine(source_name_buffer, speechBuffer, true);
            chat_display.addPartyTextLine(source_name_buffer, speechBuffer);
          } else if (actorID == 0) {
            chat_log.addSystemText(speechBuffer);
            chat_display.addSystemText(speechBuffer);
          } else {
            Actor* actor = global_actor_manager_.get(actorID);
            if (!actor) continue;
            actor->setSpeechText(speechBuffer, this_frame_time);
            if (speechBuffer[0] != '\0') {  // only add to the log if the text has length
              if (actor == global_actor_manager_.getClientActor()) {
                chat_log.addOwnChatLine(speechBuffer);
              } else {
                chat_log.addReceivedChatLine(actor->getName(), speechBuffer, false);
              }
            }
          }
        } else if (message == ::Evidyon::Network::Packet::NETMSG_CHANGEMAP) {
          int map_id;
          CONFIRM(Decode::changeMap(netEvent.packet, &map_id)) else continue;
          special_fx_manager_.onChangeMap(global_actor_manager_.getClientActor());
          global_actor_manager_.onChangeMap();
          {
            unsigned int new_background_loop = 0;
            global_map_manager_.changeMap(map_id, &new_background_loop);
            global_sound_manager_.loopBackgroundSound(new_background_loop);
          }
          { // get rid of all of the items on the map
            ItemsOnMapTable::iterator start = myItemsOnMapTable.begin();
            ItemsOnMapTable::iterator end = myItemsOnMapTable.end();
            for (ItemsOnMapTable::iterator i = start; i != end; ++i)
                mySceneryRenderer.removeScenery(i->second);
            myItemsOnMapTable.erase(start, end);
          }
        } else if (message == ::Evidyon::Network::Packet::NETMSG_TELLITEMSONMAPLOCATION) {
          int x, y;
          unsigned short item_types[MAX_ITEMS_VISIBLE_PER_LOCATION];
          int number_of_item_types;
          CONFIRM(Decode::tellItemsOnMapLocation(netEvent.packet,
                                                &x,
                                                &y,
                                                 item_types,
                                                &number_of_item_types)) else continue;
          // update the scenery here
          ItemsOnMapTable::key_type locationKey(x, y);
          {
              // Remove all of the scenery at this location
              ItemsOnMapTable::iterator start = myItemsOnMapTable.lower_bound(locationKey);
              ItemsOnMapTable::iterator end = myItemsOnMapTable.upper_bound(locationKey);
              for (ItemsOnMapTable::iterator i = start; i != end; ++i)
                  mySceneryRenderer.removeScenery(i->second);
              myItemsOnMapTable.erase(start, end);
          }

          // Create a transformation matrix for this location
          D3DXMATRIXA16 offsetMatrix;
          D3DXMatrixTranslation(&offsetMatrix, 1.0f * x + 0.5f, 0.0f, 1.0f * y + 0.5f);

          // Add new items
          size_t sceneryElements = myResources.scenery.sceneryArraySize;
          size_t itemElements = myResources.items.itemDescriptionArraySize;
          for (size_t i = 0; i < number_of_item_types; ++i)
          {
              // Get the scenery element
              //size_t itemIndex = item_types[i];
              //if (itemIndex > itemElements) continue;
              //ClientItemDescription* item = &myResources.items.itemDescriptionArray[itemIndex];
              size_t sceneryIndex = item_types[i];//item->sceneryRepresentationIndex;
              //DEBUG_INFO("Adding item on map: %i", sceneryIndex);
              if (sceneryIndex >= sceneryElements) continue;
              GameFileData_Scenery* scenery = &myResources.scenery.sceneryArray[sceneryIndex];

              // Create a random rotation matrix
              D3DXMATRIXA16 rotationMatrix;
              D3DXMatrixRotationY(&rotationMatrix, x * y + i * 0.674f);

              // Generate this element's matrix
              D3DXMATRIXA16 transform;
              D3DXMatrixMultiply(&transform, &rotationMatrix, &offsetMatrix);

              // Add this scenery
              HSCENERY handle;
              if (mySceneryRenderer.addScenery(&transform, scenery, &handle))
                  myItemsOnMapTable.insert(ItemsOnMapTable::value_type(locationKey, handle));
          }
        } else if (message == ::Evidyon::Network::Packet::NETMSG_UPDATEYOUREFFECTAVAILABILITYMASK) {
          Decode::updateYourEffectAvailabilityMask(netEvent.packet,
                                                  &myCharacter.effectAvailability);
          gameplay_controls_.syncSpells();
        }
        else if (message == ::Evidyon::Network::Packet::NETMSG_TELLYOURAVATARSTATS) {
          unsigned char level, availableAbilityPoints;
          unsigned char abilities[5];
          int hp, hpMax, mp, mpMax, exp, nextLevelExp, defense;
          AvatarRace avatar_race;
          AvatarClass avatar_class;
          if (Decode::tellYourAvatarStats(netEvent.packet, &level, abilities, &availableAbilityPoints,
              &hp, &hpMax, &mp, &mpMax, &exp, &nextLevelExp, &defense, &avatar_race, &avatar_class))
          {
              //statDisplay.update(level, abilities, availableAbilityPoints, hp, hpMax, mp, mpMax, exp, nextLevelExp, defense);
              gameplay_controls_.syncAvatarStats()->update(level,
                                                           abilities,
                                                           availableAbilityPoints,
                                                           hp,
                                                           hpMax,
                                                           mp,
                                                           mpMax,
                                                           exp,
                                                           nextLevelExp,
                                                           defense); // todo: race, class
              //if (inStatDisplay) statDisplay.show();
             /* DEBUG_INFO("Player stats:\n\tlevel:\t%i\n\tstr:\t%i\n\tagi:\t%i\n\tcon:\t%i\n\t:int:\t%i\n\twis:\t%i\n\thp:\t%i/%i\n\tmp:\t%i/%i\n\texp:\t%i/%i\n\tdefense:\t%i",
                          level, abilities[0], abilities[1], abilities[2], abilities[3], abilities[4], 
                          hp, hpMax, mp, mpMax, exp, nextLevelExp, defense);*/
          }
        } else if (message == ::Evidyon::Network::Packet::NETMSG_TELLACTORNAME) {
          ActorID actor_id = 0;
          std::string name;
          if (Decode::tellActorName(netEvent.packet,
                                    &actor_id,
                                    &name)) {
            Actor* actor = global_actor_manager_.get(actor_id);
            actor->setName(name.c_str());
          }
        } else if (message == ::Evidyon::Network::Packet::NETMSG_UPDATELOCALWORLDREIGON) {
          World::RegionID current_region_id;
          if (Decode::updateLocalWorldRegion(netEvent.packet, &current_region_id)) {

            { // remove all items that are not in a nearby region
              typedef std::set<ItemsOnMapTable::key_type> ItemsOnMapToErase;
              ItemsOnMapToErase items_on_map_to_erase;
              for (ItemsOnMapTable::iterator i = myItemsOnMapTable.begin(); i != myItemsOnMapTable.end(); ++i) {
                ItemsOnMapTable::key_type location = i->first;

                // If this location is already scheduled for erasure, we don't have to check
                // anything to know that the item here should be deleted.
                if (items_on_map_to_erase.find(location) != items_on_map_to_erase.end()) {
                  mySceneryRenderer.removeScenery(i->second);
                  continue;
                }

                short region_x, region_y;
                bool keep_this_item =
                  World::regionIDFromPoint(location.first, location.second, &region_x, &region_y) == current_region_id;

                // check neighboring regions if this item isn't in the local region
                if (!keep_this_item) {
                  World::RegionID neighbors[8] = {
                    World::regionIDFromRegionCoordinates(region_x - 1, region_y - 1),
                    World::regionIDFromRegionCoordinates(region_x - 1, region_y    ),
                    World::regionIDFromRegionCoordinates(region_x - 1, region_y + 1),
                    World::regionIDFromRegionCoordinates(region_x,     region_y - 1),
                    World::regionIDFromRegionCoordinates(region_x,     region_y + 1),
                    World::regionIDFromRegionCoordinates(region_x + 1, region_y - 1),
                    World::regionIDFromRegionCoordinates(region_x + 1, region_y    ),
                    World::regionIDFromRegionCoordinates(region_x + 1, region_y + 1),
                  };

                  for (int i = 0; i < 8 && !keep_this_item; ++i) {
                    keep_this_item = neighbors[i] == current_region_id;
                  }
                }

                // If this item should be erased, remove its scenery entry.  The location
                // will be erased in the next step.
                if (!keep_this_item) {
                  items_on_map_to_erase.insert(location);
                  mySceneryRenderer.removeScenery(i->second);
                }
              }

              // Free all items from the map table using the location keys
              for (ItemsOnMapToErase::iterator i = items_on_map_to_erase.begin(); i != items_on_map_to_erase.end(); ++i) {
                myItemsOnMapTable.erase(*i);
              }
            }
          }
        } else if (message == NETMSG_STORAGE_TELL) {
          int number_of_items = 0;
          if (Decode::storageTell(netEvent.packet, storage, &number_of_items)) {
            gameplay_controls_.syncStorageItems();
          }
        } else if (message == NETMSG_GEOSID_TELLGEONITE) {
          unsigned int geonite;
          if (Decode::geosidTellGeonite(netEvent.packet, &geonite)) {
            myCharacter.geonite = geonite;
            //DEBUG_INFO("geonite = %lu", geonite);
          }
        }
      //  else if (message == ::Evidyon::Network::Packet::NETMSG_UPDATEYOURACTIVEPARTITION)
      //  {
      //      short px, py;
      //      Decode::updateYourActivePartition(netEvent.packet, &px, &py);

      //      // Find all nearby partitions
      //      unsigned long activePartitions[9] = {
      //          Evidyon::World::regionIDFromRegionCoordinates(px - 1, py - 1),
      //          Evidyon::World::regionIDFromRegionCoordinates(px - 1, py   ),
      //          Evidyon::World::regionIDFromRegionCoordinates(px - 1, py + 1),
      //          Evidyon::World::regionIDFromRegionCoordinates(px    , py - 1),
      //          Evidyon::World::regionIDFromRegionCoordinates(px    , py   ),
      //          Evidyon::World::regionIDFromRegionCoordinates(px    , py + 1),
      //          Evidyon::World::regionIDFromRegionCoordinates(px + 1, py - 1),
      //          Evidyon::World::regionIDFromRegionCoordinates(px + 1, py   ),
      //          Evidyon::World::regionIDFromRegionCoordinates(px + 1, py + 1),
      //      };


      //      size_t removed = 0;

      //      std::set<int> actorsToRemove;

      //      // Look through all actors, remove those that aren't in one of these ID groups
      //      for (ActorTable::iterator i = myActorTable.begin(); i != myActorTable.end(); ++i)
      //      {
      //          // Never remove our actor
      //          if (i->second == myActor) continue;

      //          int p;
      //          for (p = 0; (p < 9) && (i->second->myCurrentPartition != activePartitions[p]); ++p);
      //          if (p >= 9) actorsToRemove.insert(i->first);
      //      }

      //      for (std::set<int>::iterator j = actorsToRemove.begin(); j != actorsToRemove.end(); ++j)
      //      {
      //          int actorID = *j;
      //          destroyActor(actorID);
      //      }

      //      // Look through all items and remove those that arent in one of these ID groups
      //      // TODO: this is REALLY inefficient
      //      for (ItemsOnMapTable::iterator i = myItemsOnMapTable.begin(); i != myItemsOnMapTable.end();)
      //      {
      //        unsigned long partitionID = Evidyon::World::regionIDFromPoint(i->first.first, i->first.second);
      //          int p;
      //          for (p = 0; (p < 9) && (partitionID != activePartitions[p]); ++p);
      //          if (p >= 9)
      //          {
      //              ItemsOnMapTable::iterator next = i; ++next;
      //              mySceneryRenderer.removeScenery(i->second);
      //              myItemsOnMapTable.erase(i);
      //              i = next;
      //          }
      //          else
      //              ++i;
      //      }

      //      //DEBUG_INFO("NETMSG_UPDATEYOURACTIVEPARTITION:  %lu actors removed, %lu left", actorsToRemove.size(), myActorTable.size());
      //  } else if (message == ::Evidyon::Network::Packet::NETMSG_TELLYOURMONEY) {
      //    unsigned int gold;
      //    Decode::tellYourMoney(netEvent.packet, &gold);
      //    DEBUG_INFO("NETMSG_TELLYOURMONEY:  you have %lu gold", gold);
      //    myCharacter.money = gold;
      //    bazaar_display.updateMoney(gold);
      //  } else if (message == ::Evidyon::Network::Packet::NETMSG_BAZAAR_TELLITEMSFORSALE) {
      //    ItemForTrade items_buffer[Economy::MAX_ITEMS_PER_BAZAAR_PAGE];
      //    unsigned int number_of_items = 0;
      //    Decode::bazaar_tellItemsForSale(
      //        netEvent.packet,
      //        items_buffer,
      //        Economy::MAX_ITEMS_PER_BAZAAR_PAGE,
      //        &number_of_items);
      //    /*DEBUG_INFO("NETMSG_BAZAAR_TELLITEMSFORSALE received %lu items:",number_of_items);
      //    for (int i = 0; i < number_of_items; ++i) {
      //      DEBUG_INFO("%4lu - Item %s : listed for %lu gold",
      //          items_buffer[i].listing_id,
      //          myResources.items.itemDescriptionArray[items_buffer[i].type].name,
      //          items_buffer[i].price);
      //    }*/
      //    bazaar_display.updateBazaar(items_buffer, number_of_items);
      //  } else if (message == ::Evidyon::Network::Packet::NETMSG_BAZAAR_TELLSELLERSTATUS) {
      //    ItemForTrade items_buffer[Economy::MAX_ITEMS_PER_BAZAAR_PAGE];
      //    unsigned int number_of_items = 0;
      //    Decode::bazaar_tellSellerStatus(
      //        netEvent.packet,
      //        items_buffer,
      //        Economy::MAX_ITEMS_PER_BAZAAR_PAGE,
      //        &number_of_items);
      //    /*DEBUG_INFO("NETMSG_BAZAAR_TELLSELLERSTATUS received %lu items:",number_of_items);
      //    for (int i = 0; i < number_of_items; ++i) {
      //      DEBUG_INFO(
      //          "Item %s : listed for %lu gold%s",
      //          myResources.items.itemDescriptionArray[items_buffer[i].type].name,
      //          items_buffer[i].price,
      //          items_buffer[i].purchased ? "(sold)" : "");
      //    }*/
      //    bazaar_display.updateSellerStatus(items_buffer, number_of_items);
      //  } else if (message == ::Evidyon::Network::Packet::NETMSG_BAZAAR_REMOVEITEMLISTINGS) {
      //    unsigned int listing_ids[Economy::MAX_ITEMS_PER_BAZAAR_PAGE];
      //    unsigned int num_listings = 0;
      //    Decode::bazaar_removeItemListings(netEvent.packet, listing_ids, Economy::MAX_ITEMS_PER_BAZAAR_PAGE, &num_listings);
      //    //ASSERT(num_listings == 1);
      //    bazaar_display.removeListing(listing_ids[0]);
      //  } else {
      //    DEBUG_WARNING("Invalid! %i", message);
      //  }
      //}


        // Free packet memory
        enet_packet_destroy(netEvent.packet);
      }



      Actor* myActor = global_actor_manager_.getClientActor();


      const ActionInstance* action_instance;
      bool change_cursor_state = false;
      switch (gameplay_controls_.update(this_frame_time, &action_instance, &change_cursor_state)) {
      case Action::TYPE_NPCINTERACTION: {
          const Action_NPCInteraction::Parameters* params = &action_instance->params.npc_interaction;
          switch (params->interaction_type) {
          case NPCINTERACTION_STORAGE: // open up the storage
            gameplay_controls_.showDialog(EvidyonGUIGameplayControls::DIALOG_STORAGE);
            using namespace Evidyon::Network::Packet::Client;
            enet_peer_send(myENetServer, 0, Encode::storageOpen(params->npc_actor_id));
            break;
          case NPCINTERACTION_GEOSID: // geosid sacrifice items
            gameplay_controls_.showDialog(EvidyonGUIGameplayControls::DIALOG_GEOSID);
            break;
          }
        } break;
      case Action::TYPE_MOVE: {
          const Action_Move::Parameters* params = &action_instance->params.move;
          ENetPacket* packet = NULL;
          using namespace Evidyon::Network::Packet::Client::Encode;
          if (params->streaming) {
            packet = requestMoveStreaming(params->x, params->y);
          } else {
            packet = requestMove(params->x, params->y, 0);
          }

          if (packet != NULL) enet_peer_send(myENetServer, 1, packet);
        } break;

      case Action::TYPE_ATTACK: {
          const Action_Attack::Parameters* params = &action_instance->params.attack;
          using namespace Evidyon::Network::Packet::Client::Encode;
          ENetPacket* packet = requestMove(params->x, params->y, params->actor_id);
          if (packet != NULL) enet_peer_send(myENetServer, 1, packet);
      } break;

      case Action::TYPE_SPELL: {
        const Action_CastSpell::Parameters* params = &action_instance->params.spell;
        int spell_type = action_instance->action.spell.spell_type;
        if (spell_type < MAX_SPELL_TYPES && myCharacter.effectAvailability.isBitSet(spell_type)) {
          ActorID id = 0;
          //if (params->needs_target) {
            global_actor_manager_.findActorNearLocation(params->x,
                                                        params->y,
                                                        0,
                                                        &id,
                                                        true);
          //}

          using namespace ::Evidyon::Network::Packet::Client;
          unsigned int spell_ids[3] = { spell_type, -1, -1 };
          enet_peer_send(myENetServer, 1,
            Encode::castSpell(spell_ids,
                              id,
                              params->x,
                              params->y));
        }

      } break;

      case Action::TYPE_CONSUMABLE: {
        const Action_UseConsumable::Parameters* params = &action_instance->params.consumable;
        ActorID id = 0;
        //if (params->needs_target) {
          global_actor_manager_.findActorNearLocation(params->x,
                                                      params->y,
                                                      0,
                                                      &id,
                                                      true);
        //}
        enet_peer_send(myENetServer, 0, Encode::useItem(params->inventory_index_hint,
                                                        id,
                                                        params->x,
                                                        params->y));
      } break;

      case Action::TYPE_USEREVENT: {
          switch (action_instance->action.user_event) {
          case STORAGE_DEPOSIT_ITEM: {
            using namespace Evidyon::Network::Packet::Client;
            enet_peer_send(myENetServer, 0, Encode::storageDeposit(0, action_instance->params.user_event.data_.item_event_.inventory_index_));
            } break;
          case STORAGE_WITHDRAW_ITEM: {
            using namespace Evidyon::Network::Packet::Client;
            enet_peer_send(myENetServer, 0, Encode::storageWithdraw(0, action_instance->params.user_event.data_.item_event_.inventory_index_));
            } break;
          case EVENT_ESCAPE: {
              if (gameplay_controls_.hideDialogs() == false) {
                // log out of world
                myStateMachine.jumpState(VCS_LOGGED_INTO_ACCOUNT);
                enet_peer_send(myENetServer, 0, ::Evidyon::Network::Packet::Client::Encode::leaveWorld());
              }
            } break;
            case TOGGLE_CHAT_LOG: {
              if (chat_log.visible()) {
                chat_log.hide();
                chat_display.show(&gui_primary_layer_);
              } else {
                chat_display.hide();
                chat_log.show(&gui_primary_layer_);
              }
            } break;

            case OPEN_INVENTORY:    gameplay_controls_.toggleDialog(EvidyonGUIGameplayControls::DIALOG_INVENTORY); break;
            case OPEN_STAT_DISPLAY: gameplay_controls_.toggleDialog(EvidyonGUIGameplayControls::DIALOG_STAT_DISPLAY); break;
            case OPEN_DROP:         gameplay_controls_.toggleDialog(EvidyonGUIGameplayControls::DIALOG_DROP_ITEMS); break;
            case OPEN_CAST:         gameplay_controls_.toggleDialog(EvidyonGUIGameplayControls::DIALOG_CAST_SPELL); break;
            case OPEN_USEITEM:      gameplay_controls_.toggleDialog(EvidyonGUIGameplayControls::DIALOG_USE_ITEM); break;
            case OPEN_CHAT:         gameplay_controls_.toggleDialog(EvidyonGUIGameplayControls::DIALOG_CHAT); break;

            case ATTRIBUTES_ADD_ABILITY_POINT: {
              using namespace Evidyon::Network::Packet::Client;
              enet_peer_send(myENetServer,
                             0,
                             Encode::askMyAvatarStats(action_instance->params.user_event.data_.add_ability_point_.ability));
            } break;

            case DROP_ITEM: {
              unsigned char itemIndex = action_instance->params.user_event.data_.item_event_.inventory_index_;
              enet_peer_send(myENetServer, 1, ::Evidyon::Network::Packet::Client::Encode::dropInventoryItems(&itemIndex, 1));
              myCharacter.inventory[itemIndex].quantity = 0;
//              item_drop_screen.update(myCharacter.inventory, myCharacter.inventory_update_indices_);
              gameplay_controls_.syncInventory();
            } break;

            case SPEAK: {
              const char* tokens[] = {
                "stats",
                "help",
                "/",
              };
              bool found_match = false;
              if (action_instance->params.user_event.data_.speak_.text_[0] == '/') {
                for (int i = 0; i < sizeof(tokens)/sizeof(const char*); ++i) {
                  if (0 != strncmp(tokens[i],
                                  &action_instance->params.user_event.data_.speak_.text_[1],
                                   strlen(tokens[i]))) {
                    continue;
                  }

                  found_match = true;

                  switch (i) {
                    case 0: { // "/stats"
                      // print stats
                      enum Stats {
                        ACTORS,
                        NETWORK,
                        MAPS,
                        NUMBER_OF_STATS
                      };
                      for (int i = 0; i < NUMBER_OF_STATS; ++i) {
                        char buffer[512];
                        switch ((Stats)i) {
                          case ACTORS:  global_actor_manager_.printDebugStats(buffer, 512); break;
                          case NETWORK:
                            sprintf_s(buffer, 512,
                                      "Network: %lu Bps incoming / %lu Bps outgoing (%lu:%lu loss);  %lu ms average RTT/%lu ms best RTT/%lu variance",
                                      myENetServer->incomingDataTotal,
                                      myENetServer->outgoingDataTotal,
                                      myENetServer->packetLoss,
                                      ENET_PEER_PACKET_LOSS_SCALE,
                                      myENetServer->roundTripTime,
                                      myENetServer->lowestRoundTripTime,
                                      myENetServer->roundTripTimeVariance);
                            break;
                          case MAPS:    global_map_manager_.printDebugStats(buffer, 512); break;
                        }
                        DEBUG_INFO("/stats:  %s", buffer);
                        chat_log.addSystemText(buffer);
                        chat_display.addSystemText(buffer);
                      }
                    } break;

                    case 1: { // "/help"
                      std::string help("/stats - display debugging information,  //*** - write *** to the debug log");
                      chat_log.addSystemText(help);
                      chat_display.addSystemText(help);
                    } break;

                    case 2: { // "//[text]"
                      std::string message = &action_instance->params.user_event.data_.speak_.text_[strlen(tokens[i]) + 1];
                      DEBUG_INFO("%s", message.c_str());
                      chat_log.addSystemText(message);
                      chat_display.addSystemText(message);
                    } break;
                  }

                  // found a match
                  break;
                }

                // don't send to the server if we matched
                if (found_match) break;
              }
              last_spoken_text_line = action_instance->params.user_event.data_.speak_.text_;
              if (false == last_spoken_text_line.empty()) {
                using namespace ::Evidyon::Network::Packet::Client;
                enet_peer_send(myENetServer,
                               1,
                               Encode::requestSpeak(action_instance->params.user_event.data_.speak_.text_));
              }
            } break;

            case SAY_AGAIN: {
              if (last_spoken_text_line.empty()) break; // don't send nothing
              using namespace ::Evidyon::Network::Packet::Client;
              enet_peer_send(myENetServer,
                             1,
                             Encode::requestSpeak(action_instance->params.user_event.data_.speak_.text_));
            } break;

            case UNEQUIP_ITEM:
            case EQUIP_ITEM: {
              using namespace ::Evidyon::Network::Packet::Client;
              unsigned char itemIndices[128];
              size_t selectedItems = 1;
              itemIndices[0] = action_instance->params.user_event.data_.item_event_.inventory_index_;
              enet_peer_send(myENetServer,
                             1,
                             Encode::changeEquippedState(itemIndices, selectedItems));

              // Predict update
              int highest_index = ++myCharacter.highest_inventory_update_index_;
              for (int i = 0; i < selectedItems; ++i) {
                myCharacter.inventory_update_indices_[itemIndices[i]] = highest_index;
              }
//              item_drop_screen.update(myCharacter.inventory,myCharacter.inventory_update_indices_);
//              inventory.updateInventory(myCharacter.inventory,myCharacter.inventory_update_indices_);

              gameplay_controls_.syncInventory();

            } break;

            case GEOSID_SACRIFICE_ITEM: {
              using namespace ::Evidyon::Network::Packet::Client;
              enet_peer_send(myENetServer,
                             1,
                             Encode::geosidSacrifice(action_instance->params.user_event.data_.item_event_.inventory_index_));
            } break;

            case TAKE_SCREENSHOT: {
              take_screenshot = true; // this has to be done just after writing image data
            } break;

          }
        } break;
      }

      if (change_cursor_state) { // set by game controls::update()
        if (0 == gameplay_controls_.getCursorState()) {
          setMouseCursorNormal();
        } else {
          setMouseCursorActive();
        }
      }


      static float lastTargetX = myActor->getX(),
                   lastTargetY = myActor->getY();

      // If the HP, MP or EXP values changed, update the canvases that visualize them
      if (changedHPMPEXP) {
        status_bars.set(myCharacter.currentHPPercent,
                        myCharacter.currentMPPercent,
                        myCharacter.currentEXPPercent);

        changedHPMPEXP = false;
      }

      { // Handle input from the different menu sources
        chat_display.update(this_frame_time);
      }

      //static bool s = false;
      //if (myKeyboardInputSource.isKeyDown(GUIKEY_G)) {
      //  if (!s) {
      //    s = true;
      //    gameplay_controls_.showDialog(EvidyonGUIGameplayControls::DIALOG_GEOSID);
      //  }
      //} else {
      //  s = false;
      //}

      // Update the sound
      global_sound_manager_.update(myActor->getX(), myActor->getY());

      // Update locations of things that only need to be done once / visible frame
      bool render_current_frame = allowFrameToBeRendered(this_frame_time);
      if (render_current_frame) {
        double time_since_last_rendered_frame = this_frame_time - last_rendered_frame_time;
        time_since_last_rendered_frame = min(time_since_last_rendered_frame, MAX_FRAME_DELAY);
        last_rendered_frame_time = this_frame_time;
        current_fps = (FPS_EXP_WEIGHT) * (1.0 / time_since_last_rendered_frame) +
                      (1.0 - FPS_EXP_WEIGHT) * current_fps;


        // Advance the actors' states.  Do this before advancing special effects
        // to make sure moving actors dont make weird things happen
        global_actor_manager_.update(this_frame_time, time_since_last_rendered_frame);
        if (global_actor_manager_.actorsNeedDescriptions()) {
          enet_peer_send(myENetServer, 0, global_actor_manager_.buildRequestActorDescriptionPacket());
        }

        // Set the camera location
        {
          float x = myActor->getX();
          float y = myActor->getY();
          myIsometricCamera.setPosition(x, y);
          myIsometricCamera.rebuildXenimusCamera(myD3DParams.BackBufferWidth,
                                                 myD3DParams.BackBufferHeight);
          global_map_manager_.update(x, y);
        }

        { // update the mouse-over-actor
          Evidyon::ActorID actor_id;
          float x, y;
          GUIPoint mouse = myMouseInputSource.getMousePosition();
          myIsometricCamera.getWorldLocation(mouse.x, mouse.y, &x, &y);
          global_actor_manager_.findActorNearLocation(x, y, 0, &actor_id, false);
          Actor* actor = global_actor_manager_.get(actor_id);
          mouse_over_name_display.setText(actor == NULL ? "" : actor->getName());
        }

        special_fx_manager_.update(this_frame_time, time_since_last_rendered_frame);


        // Set up a material. The material here just has the diffuse and ambient
        // colors set to yellow. Note that only one material can be used at a time.
        D3DMATERIAL9 mtrl;
        ZeroMemory(&mtrl, sizeof(D3DMATERIAL9));
        mtrl.Diffuse.r = mtrl.Ambient.r = 1.0f;
        mtrl.Diffuse.g = mtrl.Ambient.g = 1.0f;
        mtrl.Diffuse.b = mtrl.Ambient.b = 1.0f;
        mtrl.Diffuse.a = mtrl.Ambient.a = 1.0f;
        myD3DDevice->SetMaterial(&mtrl);

        // Set up a white, directional light, with an oscillating direction.
        // Note that many lights may be active at a time (but each one slows down
        // the rendering of our scene). However, here we are just using one. Also,
        // we need to set the D3DRS_LIGHTING renderstate to enable lighting
        {
          //D3DXVECTOR3 vecDir;
          //D3DLIGHT9 light;
          //ZeroMemory(&light, sizeof(D3DLIGHT9));
          //light.Type = D3DLIGHT_SPOT;
          //light.Diffuse.r = 1.0f;
          //light.Diffuse.g = 1.0f;
          //light.Diffuse.b = 1.0f;

          //GUIPoint coord = myMouseInputSource.getMousePosition();
          //myIsometricCamera.getWorldLocation(coord.x, coord.y, &light.Position.x, &light.Position.z);
          //light.Position.x -= interpolatedViewX;
          //light.Position.z -= interpolatedViewY;
          //light.Position.y = 20.0f;
          //light.Attenuation0 = 0.0f;
          //light.Attenuation1 = 2.1f;
          //light.Attenuation2 = 0.0f;
          //light.Falloff = 1.0f;

          //light.Direction.x =  0.0f;
          //light.Direction.y = -1.0f;
          //light.Direction.z =  0.0f;
          //light.Range = 1000.0f;

          //light.Theta = atan2f((myIsometricCamera.topdownViewEnabled() ? 40.0f : 5.0f)/2.0f, light.Position.y);
          //light.Phi = atan2f((myIsometricCamera.topdownViewEnabled() ? 40.0f : 6.0f)/2.0f, light.Position.y);

          //myD3DDevice->SetLight(0, &light);
          //myD3DDevice->LightEnable(0, TRUE);
          //light.Position.x = 0.0f;
          //light.Position.z = 0.0f;
          //light.Attenuation1 = 1.0f;
          //myD3DDevice->SetLight(1, &light);
          //myD3DDevice->LightEnable(1, TRUE);
        }

        myD3DDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
        //myD3DDevice->SetRenderState(D3DRS_NORMALIZENORMALS, TRUE);
        myD3DDevice->SetRenderState(D3DRS_NORMALIZENORMALS, FALSE);


        // Finally, turn on some ambient light.
        BYTE b = (BYTE)fabsf(cosf((this_frame_time - time_of_day_adjustment_)/(60.0*5)) * 255.0);
        if (b > 0xB0) b = 0xB0;
        if (b < 0x55) b = 0x55;
        //b = 0x65;
        //b = 0xFF;
        myD3DDevice->SetRenderState(D3DRS_AMBIENT, D3DCOLOR_XRGB(b,b,b));
        //myD3DDevice->SetRenderState(D3DRS_AMBIENT, D3DCOLOR_XRGB(0xFF,0xFF,0xFF));

        myD3DDevice->Clear(0, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(0,0,0), 1.0f, 0);



        //myD3DDevice->SetRenderState( D3DRS_FOGENABLE, TRUE );
        //myD3DDevice->SetRenderState( D3DRS_FOGTABLEMODE, D3DFOG_LINEAR );
        //myD3DDevice->SetRenderState( D3DRS_FOGCOLOR, D3DCOLOR_XRGB(128, 128, 205) );
        //{
        //  float f;
        //  myD3DDevice->SetRenderState( D3DRS_FOGSTART, *(DWORD*)&(f=0.15f) );
        //  myD3DDevice->SetRenderState( D3DRS_FOGEND, *(DWORD*)&(f=0.30f) );
        //}


        // Begin the scene
        if (SUCCEEDED(myD3DDevice->BeginScene())) {
          // Initialize rendering
          myIsometricCamera.initialize3DRendering(myD3DDevice);

          // Draw the 3D scene
          for (unsigned int i = 0; i < global_rendering_manager_.numberOfTextures(); ++i) {
            CONFIRM(mySceneryRenderer.renderTextureSubset(&myIsometricCamera, i, &global_rendering_manager_));
            CONFIRM(global_map_manager_.render(&myIsometricCamera, i));
            special_fx_manager_.render(i, &myIsometricCamera, &global_rendering_manager_);

            // actor lighting only works in software mode :/
            CONFIRM(global_actor_manager_.render(myD3DDevice, i, &myIsometricCamera, &global_rendering_manager_));
          }

          // Don't fog during special effects; it looks really weird
          myD3DDevice->SetRenderState(D3DRS_FOGENABLE, FALSE);

          global_rendering_manager_.resetCurrentTexture();

          //if (light_texture != NULL) { // can be used to disable...
          if (false) {
            myD3DDevice->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
            myD3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
            myD3DDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCCOLOR);
            myD3DDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);
            myD3DDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
            myD3DDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
            myD3DDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );
            myD3DDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );
            myD3DDevice->SetSamplerState( 0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR );

            myD3DDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_BLENDDIFFUSEALPHA);
            myD3DDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
            myD3DDevice->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
            myD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
            myD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_DIFFUSE);

            // Render a plane at src
            const float height = 0.0f;
            //int amt = (GetTickCount()/1000.0f) * (255*2);
            //BYTE alpha = amt > 255 ? (255*2-amt) : amt;//(BYTE)(cosf((this_frame_time - time_of_day_adjustment_)/(60.0*5)) * 255.0);
            BYTE alpha = (BYTE)(cosf((this_frame_time - time_of_day_adjustment_)/(60.0*5)) * 255.0);
            D3DCOLOR color = D3DCOLOR_ARGB(255-alpha,alpha,alpha,alpha);
            struct LightVertex {
              FLOAT x, y, z;
              float rhw;
              D3DCOLOR diffuse;
              FLOAT u, v;
            };
            static const unsigned long D3DFVF_LIGHTVERTEX = (D3DFVF_XYZRHW|D3DFVF_TEX1|D3DFVF_DIFFUSE);
            const float factor = 1.65f;
            const float center_x = 400.0f;
            const float center_y = 300.0f;
            LightVertex plane[] = {
                { center_x-center_x*factor, center_y-center_y*factor, 0.0f, 0.5f, color,  0.0f, 0.0f },
                { center_x+center_x*factor, center_y-center_y*factor, 0.0f, 0.5f, color, 1.0f, 0.0f },
                { center_x-center_x*factor, center_y+center_y*factor, 0.0f, 0.5f, color, 0.0f, 1.0f },
                { center_x+center_x*factor, center_y+center_y*factor, 0.0f, 0.5f, color, 1.0f, 1.0f },
            };

            myD3DDevice->SetFVF( D3DFVF_LIGHTVERTEX );

            // multiply effect
            myD3DDevice->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
            myD3DDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_DESTCOLOR);
            myD3DDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_SRCCOLOR);

            global_rendering_manager_.changeCurrentD3DStage0Texture(light_texture);
            myD3DDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, plane, sizeof(LightVertex));

            myD3DDevice->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
            myD3DDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
            myD3DDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_DIFFUSE);
            myD3DDevice->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_TEXTURE);
            myD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
            myD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);

            myD3DDevice->SetRenderState(D3DRS_ZENABLE, TRUE);
            myD3DDevice->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
            myD3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
            myD3DDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
            myD3DDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
            myD3DDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_POINT );
            myD3DDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_POINT );
            myD3DDevice->SetSamplerState( 0, D3DSAMP_MIPFILTER, D3DTEXF_POINT );
          }

          special_fx_manager_.renderFX(&myIsometricCamera);


          // Draw the new GUI elements
          CONFIRM(SUCCEEDED(myD3DDevice->Clear(0, NULL, D3DCLEAR_ZBUFFER, 0, 1.0f, 0)));

          // Switch to the font texture
          gui_primary_layer_.reinitializeD3DDeviceState();
          global_rendering_manager_.changeCurrentD3DStage0Texture(speechFont.getTexture());

          { // Display player speech
            GlobalActorManager::ActorMap::const_iterator begin = global_actor_manager_.allActorsBegin();
            GlobalActorManager::ActorMap::const_iterator end = global_actor_manager_.allActorsEnd();
            for (GlobalActorManager::ActorMap::const_iterator i = begin; i != end; ++i) {
                Actor* actor = i->second;
                const char* speech = actor->getSpeech();
                if (speech == NULL) continue;
                float x, y;
                myIsometricCamera.getScreenLocation(actor->getX(),
                                                    2.0f,           // the height of the actor's head
                                                    actor->getY(),
                                                    &x,
                                                    &y);
                GUIRect area = { (short)x, (short)y,
                                 (short)x, (short)y };
                GUIPoint pt;
                speechFont.centerTextInArea(speech,
                                            -1,
                                            &area,
                                            &pt);
                CONFIRM(speechFont.renderText(&gui_primary_layer_,
                                              NULL,  // no clipping rectangle
                                              pt.x,
                                              pt.y,
                                              1.0f, // depth
                                              speech,
                                              -1, // null-terminated
                                              D3DCOLOR_XRGB(255,255,255))); // rendering color
              }


            gui_primary_layer_.flush();
          }

          { // Draw the amount of geonite a player has
            char geonite_text[64];
            sprintf_s(geonite_text, 64, "%lu", myCharacter.geonite);
            GUISize size;
            speechFont.getTextExtent(geonite_text, -1, &size);
            const GUIRect* screen = gui_primary_layer_.getPrimaryCanvas()->getArea();
            speechFont.renderText(&gui_primary_layer_,
                                  NULL,
                                  screen->right - size.width,
                                  screen->bottom - size.height,
                                  0.9f,
                                  geonite_text,
                                  -1,
                                  D3DCOLOR_XRGB(64,180,64));
          }

          CONFIRM(gui_primary_layer_.render());




          // End the scene
          myD3DDevice->EndScene();

          // this is ugly, but it has to be done here because it is the only place the back-buffer has the
          // correct image data in it.
          if (take_screenshot) {
            take_screenshot = false;

            LPDIRECT3DSURFACE9 back_buffer_surface;
            HRESULT hr = myD3DDevice->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &back_buffer_surface);
            CONFIRM(SUCCEEDED(hr)) else break;
            char date[MAX_PATH];
            SYSTEMTIME st;
            GetSystemTime(&st);
            GetDateFormatA(LOCALE_USER_DEFAULT,
                           0,
                           &st,
                           "yyyy-MM-dd",
                           date,
                           sizeof(date));
            char file_name[MAX_PATH];
            sprintf_s(file_name, sizeof(file_name), "%s - %lu%lu%lu.png", date, st.wHour, st.wMinute, st.wSecond);
            hr = D3DXSaveSurfaceToFile(file_name,
                                       D3DXIFF_PNG,
                                       back_buffer_surface,
                                       NULL,
                                       NULL);
            SAFE_RELEASE(back_buffer_surface);
            CONFIRM(SUCCEEDED(hr)) else break;


            {
              std::string message = "Screenshot saved as \"";
              message.append(file_name);
              message.append("\"");
              DEBUG_INFO("%s", message.c_str());
              chat_log.addSystemText(message);
              chat_display.addSystemText(message);
            }
          }
        }

        if (render_current_frame) {
            // Present the backbuffer contents to the display
            HRESULT hr;
            CONFIRM(SUCCEEDED(hr = myD3DDevice->Present(NULL, NULL, NULL, NULL))) else
            {
                myStateMachine.jumpState(VCS_LOST_D3DDEVICE);
                myStateMachine.queueState(VCS_LOGGED_INTO_ACCOUNT);
                enet_peer_send(myENetServer, 0, ::Evidyon::Network::Packet::Client::Encode::leaveWorld());
            }
        }
      }
    }


    SetCursor(LoadCursor(NULL, IDC_ARROW));
    ShowCursor(TRUE);

    //chat_text_input.destroy();
    chat_log.destroy();
    chat_display.destroy();

    // Free class-based information used in this method
    global_actor_manager_.reset();
    mySceneryRenderer.clear();
    global_rendering_manager_.resetCurrentTexture();
    special_fx_manager_.clear();

    gameplay_controls_.destroy();

    gui_primary_layer_.unregisterElement(&mouse_over_name_display);

    status_bars.unregisterElements(&gui_primary_layer_);

    enchantment_icons.unregisterElements(&gui_primary_layer_);
    
    gui_primary_layer_.removeTexture(font.getTextureID());
    gui_primary_layer_.removeTexture(speechFont.getTextureID());

    speechFont.destroy();
    font.destroy();

    global_sound_manager_.loopBackgroundSound(-1); // turn off background sound

    // Make sure the mouse goes back to its regular state
    myMouseInputSource.setMouseLock(false);
    setMouseCursorNormal();

SAFE_RELEASE(light_texture);
}