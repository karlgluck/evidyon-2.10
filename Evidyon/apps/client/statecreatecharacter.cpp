//------------------------------------------------------------------------------------------------
// File:    statecreatecharacter.cpp
//
// Desc:    
//
// Copyright (c) 2007-2008 Unseen Studios.   All rights reserved.
//------------------------------------------------------------------------------------------------
#include "stdafx.h"

#include "../common/GUI/guicanvasrowarray.h"
#include "volucrisguibutton.h"
#include "volucrisguieditabletextline.h"
#include "../common/GUI/guitextlabelelement.h"
#include "../common/GUI/guiscrollablelist.h"
#include "../common/GUI/guicanvasmatrix.h"
#include "../common/GUI/guicomponentset.h"
#include "evidyonguitooltipbox.h"
#include "volucrisguidefs.h"





class EvidyonGUITextLabel : public GUIComponent {
public:
  void setFont(GUIFont* font);

  void setText(const std::string& text, GUISize* panel_size);
  virtual void registerElements(GUILayer* layer);
  virtual void unregisterElements(GUILayer* layer);
private:
  GUITextLabelElement description_;
};


void EvidyonGUITextLabel::setText(const std::string& text, GUISize* panel_size) {
  {
    GUIFont* font = description_.getFont();
    GUISize size;
    font->getTextExtent(text.c_str(), text.length(), &size);
    size.width += 16;
    size.height += 16;
    *panel_size = size;
  }
  description_.setText(text.c_str());
  description_.setAlignment(GUIALIGN_LEFT_TOP);
}

void EvidyonGUITextLabel::setFont(GUIFont* font) {
  description_.setFont(font);
  description_.setAlignment(GUIALIGN_LEFT);
  getCanvas()->relativeWidth(-5);
  getCanvas()->relativeHeight(-5);
  getCanvas()->relativeDepth(-0.01f);
}


void EvidyonGUITextLabel::registerElements(GUILayer* layer) {
  GUICanvas* canvas = getCanvas();
  canvas->alignXCenter(0);
  canvas->alignYCenter(0);
  canvas->relativeWidth(-16);
  canvas->relativeHeight(-16);
  canvas->relativeDepth(-0.01f);
  description_.setSourceCanvas(canvas);
  layer->registerElement(&description_);
}

void EvidyonGUITextLabel::unregisterElements(GUILayer* layer) {
  layer->unregisterElement(&description_);
}






//------------------------------------------------------------------------------------------------
// Name:  stateCreateCharacter
// Desc:  
//------------------------------------------------------------------------------------------------
void VolucrisClient::stateCreateCharacter()
{
using namespace ::Evidyon::Network::Packet;
using namespace ::Evidyon::Network::Packet::Client;

  GUICanvas full_canvas;
  full_canvas.absoluteWidth(800);
  full_canvas.absoluteHeight(600);
  full_canvas.alignXCenter(0);
  full_canvas.alignYCenter(0);
  full_canvas.absoluteDepth(1.0f);
  full_canvas.setSourceCanvas(gui_primary_layer_.getPrimaryCanvas());

  //typedef std::set<GUIComponent*> GUIComponentSet;
  typedef std::set<GUIElement*> GUIElementSet;
  GUIElementSet elements;
  GUIComponentSet components;

  GUIScrollableList races_list, classes_list;
  VolucrisGUIButton* races = new VolucrisGUIButton[myResources.avatars.raceArraySize];
  ASSERT(races) else return;
  VolucrisGUIButton* classes = new VolucrisGUIButton[myResources.avatars.classArraySize];
  ASSERT(races) else { SAFE_DELETE_ARRAY(races); return; }

  EvidyonGUITooltipBox tooltip_box;

  VolucrisGUIEditableTextLine name;
  GUITextLabelElement name_label;
  GUICanvas name_label_canvas;
  {
    name.getCanvas()->setSourceCanvas(&full_canvas);
    name.getCanvas()->alignXLeft(100);
    name.getCanvas()->alignYTop(10);
    name.getCanvas()->absoluteWidth(600);
    name.getCanvas()->absoluteDepth(0.5f);
    name.getCanvas()->absoluteHeight(35);
    name.configure(&myWindowFont,
                   false,
                   false,
                   VolucrisGUIDefs::CONTENT_COMPONENT_TEXT_COLOR,
                   MAX_AVATAR_NAME_LENGTH);
    components.add(&name);

    name_label_canvas.setSourceCanvas(&full_canvas);
    name_label_canvas.alignXLeft(0);
    name_label_canvas.alignYTop(0);
    name_label_canvas.absoluteWidth(90);
    name_label_canvas.absoluteHeight(35);
    name_label.setSourceCanvas(&name_label_canvas);
    name_label.setFont(&myWindowFont);
    name_label.setText("Name:  ");
    name_label.setAlignment(GUIALIGN_RIGHT);
    name_label.setTextColor(VolucrisGUIDefs::CONTENT_COMPONENT_TEXT_COLOR);
    elements.insert(&name_label);
  }

  GUITextLabelElement description;
  GUICanvas description_canvas;
  {
    description_canvas.setSourceCanvas(&full_canvas);
    description_canvas.relativeWidth(0);
    description_canvas.absoluteHeight(35);
    description_canvas.alignXCenter(0);
    description_canvas.alignYTop(10+35+10);
    description_canvas.absoluteDepth(0.5f);

    description.setAlignment(GUIALIGN_CENTER);
    description.setSourceCanvas(&description_canvas);
    description.setFont(&myWindowFont);
    description.setTextColor(VolucrisGUIDefs::CONTENT_COMPONENT_TEXT_COLOR);
    elements.insert(&description);
  }

  GUITextLabelElement errors;
  GUICanvas errors_canvas;
  {
    errors_canvas.setSourceCanvas(&full_canvas);
    errors_canvas.relativeWidth(0);
    errors_canvas.absoluteHeight(35);
    errors_canvas.alignXCenter(0);
    errors_canvas.alignYBottom(-50);
    errors_canvas.absoluteDepth(0.5f);

    errors.setAlignment(GUIALIGN_CENTER);
    errors.setSourceCanvas(&errors_canvas);
    errors.setFont(&myWindowFont);
    errors.setTextColor(VolucrisGUIDefs::CONTENT_COMPONENT_TEXT_COLOR);
    elements.insert(&errors);
  }


  GUICanvas stats, stat_points_left_text_canvas;
  GUICanvasMatrix<1,2> genders_matrix;
  GUICanvasRowArray stats_text_rows, stats_value_rows, stats_plus_rows, stats_minus_rows;
  VolucrisGUIButton back, next, genders[2], stats_plus[5], stats_minus[5];
  GUITextLabelElement stat_points_left_text, stats_text[5], stats_value_text[5];

  static const char* GENDERS_TEXT[] = { "Male", "Female" };
  {
    genders_matrix.absoluteDepth(0.5f);
    genders_matrix.setSourceCanvas(&full_canvas);
    genders_matrix.absoluteWidth(300);
    genders_matrix.absoluteHeight(35);
    genders_matrix.alignXLeft(120);
    genders_matrix.alignYTop(10+35+10+35+10);

    genders[0].getCanvas()->setSourceCanvas(genders_matrix.getCellCanvas(0,0));
    genders[1].getCanvas()->setSourceCanvas(genders_matrix.getCellCanvas(0,1));

    for (int i = 0; i < 2; ++i) {
      genders[i].getCanvas()->alignXCenter(0);
      genders[i].getCanvas()->alignYCenter(0);
      genders[i].getCanvas()->relativeWidth(-6);
      genders[i].getCanvas()->relativeHeight(0);
      genders[i].getCanvas()->relativeDepth(0.0f);
      genders[i].setFont(&myWindowFont);
      genders[i].setText(GENDERS_TEXT[i]);
      components.add(&genders[i]);
    }
  }

  {
    races_list.setLineHeight(35);
    races_list.getCanvas()->setSourceCanvas(&full_canvas);
    races_list.getCanvas()->alignYTop(10+35+10+35+10+35+10);
    races_list.getCanvas()->alignXLeft(10);
    races_list.getCanvas()->absoluteWidth(200);
    races_list.getCanvas()->relativeHeight(0);
    races_list.getCanvas()->absoluteDepth(0.5f);
    components.add(&races_list);

    for (int i = 0; i < myResources.avatars.raceArraySize; ++i) {
      races[i].setText(myResources.avatars.raceArray[i].name.c_str());
      races[i].setFont(&myWindowFont);
      races[i].getCanvas()->alignXCenter(0);
      races[i].getCanvas()->alignYCenter(0);
      races[i].getCanvas()->relativeWidth(0);
      races[i].getCanvas()->relativeHeight(0);
      races[i].getCanvas()->relativeDepth(0.0f);
      races_list.setComponent(i, &races[i]);
    }
  }

  {
    classes_list.setLineHeight(35);
    classes_list.getCanvas()->setSourceCanvas(&full_canvas);
    classes_list.getCanvas()->alignYTop(10+35+10+35+10+35+10);
    classes_list.getCanvas()->alignXLeft(10+200+10);
    classes_list.getCanvas()->absoluteDepth(0.5f);
    classes_list.getCanvas()->absoluteWidth(200);
    classes_list.getCanvas()->relativeHeight(0);
    components.add(&classes_list);

    for (int i = 0; i < myResources.avatars.classArraySize; ++i) {
      classes[i].setText(myResources.avatars.classArray[i].name.c_str());
      classes[i].setFont(&myWindowFont);
      classes[i].getCanvas()->alignXCenter(0);
      classes[i].getCanvas()->alignYCenter(0);
      classes[i].getCanvas()->relativeWidth(0);
      classes[i].getCanvas()->relativeHeight(0);
      classes[i].getCanvas()->relativeDepth(0.0f);
      classes_list.setComponent(i, &classes[i]);
    }
  }

  {
    back.setText("Back");
    back.getCanvas()->setSourceCanvas(&full_canvas);
    back.getCanvas()->alignXLeft(5);
    back.getCanvas()->alignYBottom(-10);
    back.getCanvas()->absoluteWidth(310);
    back.getCanvas()->absoluteHeight(35);
    back.getCanvas()->absoluteDepth(0.5f);
    back.setFont(&myWindowFont);
    components.add(&back);
  }
  {
    next.setText("Create Character");
    next.getCanvas()->setSourceCanvas(&full_canvas);
    next.getCanvas()->alignXRight(-5);
    next.getCanvas()->alignYBottom(-10);
    next.getCanvas()->absoluteWidth(310);
    next.getCanvas()->absoluteHeight(35);
    next.getCanvas()->absoluteDepth(0.5f);
    next.setFont(&myWindowFont);
    components.add(&next);
  }

  {
    stats.absoluteWidth(355);
    stats.absoluteHeight(500);
    stats.alignXLeft(430);
    stats.alignYTop(10+35+10+35+10+35+10);
    stats.absoluteDepth(0.5f);
    stats.setSourceCanvas(&full_canvas);

    stat_points_left_text_canvas.setSourceCanvas(&stats);
    stat_points_left_text_canvas.relativeWidth(0);
    stat_points_left_text_canvas.absoluteHeight(35);
    stat_points_left_text_canvas.alignXLeft(0);
    stat_points_left_text_canvas.alignYTop(-35);
    stat_points_left_text_canvas.relativeDepth(0.0f);

    stats_plus_rows.absoluteWidth(35);
    stats_plus_rows.relativeDepth(0.0f);
    stats_plus_rows.alignXRightFromRight(0);
    stats_plus_rows.alignYTopFromTop(0);
    stats_plus_rows.setSourceCanvas(&stats);
    stats_plus_rows.setRowHeight(35);

    for (int i = 0; i < 5; ++i) {
      stats_plus[i].getCanvas()->setSourceCanvas(stats_plus_rows.getRowCanvas(i));
      stats_plus[i].getCanvas()->alignXCenter(0);
      stats_plus[i].getCanvas()->alignYCenter(0);
      stats_plus[i].getCanvas()->relativeWidth(-2);
      stats_plus[i].getCanvas()->relativeHeight(-2);
      stats_plus[i].getCanvas()->relativeDepth(0.0f);
      stats_plus[i].setFont(&myWindowFont);
      stats_plus[i].setText("+");
      components.add(&stats_plus[i]);
    }

    stats_minus_rows.absoluteWidth(35);
    stats_minus_rows.relativeDepth(0.0f);
    stats_minus_rows.alignXRightFromRight(-35);
    stats_minus_rows.alignYTopFromTop(0);
    stats_minus_rows.setSourceCanvas(&stats);
    stats_minus_rows.setRowHeight(35);

    for (int i = 0; i < 5; ++i) {
      stats_minus[i].getCanvas()->setSourceCanvas(stats_minus_rows.getRowCanvas(i));
      stats_minus[i].getCanvas()->alignXCenter(0);
      stats_minus[i].getCanvas()->alignYCenter(0);
      stats_minus[i].getCanvas()->relativeWidth(-2);
      stats_minus[i].getCanvas()->relativeHeight(-2);
      stats_minus[i].getCanvas()->relativeDepth(0.0f);
      stats_minus[i].setFont(&myWindowFont);
      stats_minus[i].setText("-");
      components.add(&stats_minus[i]);
    }

    stats_value_rows.absoluteWidth(50);
    stats_value_rows.relativeDepth(0.0f);
    stats_value_rows.alignXRightFromRight(-2*35);
    stats_value_rows.alignYTopFromTop(0);
    stats_value_rows.setSourceCanvas(&stats);
    stats_value_rows.setRowHeight(35);

    stats_text_rows.relativeWidth(-2*35-50);
    stats_text_rows.relativeDepth(0.0f);
    stats_text_rows.alignXLeftFromLeft(0);
    stats_text_rows.alignYTopFromTop(0);
    stats_text_rows.setSourceCanvas(&stats);
    stats_text_rows.setRowHeight(35);

    stat_points_left_text.setSourceCanvas(&stat_points_left_text_canvas);
    stat_points_left_text.setFont(&myWindowFont);
    stat_points_left_text.setTextColor(VolucrisGUIDefs::CONTENT_COMPONENT_TEXT_COLOR);
    stat_points_left_text.setAlignment(GUIALIGN_LEFT);
    elements.insert(&stat_points_left_text);

    const char* STATS_TEXT[] =
        { "Strength", "Agility", "Constitution", "Intelligence", "Wisdom" };

    for (int i = 0; i < 5; ++i) {
      stats_text[i].setSourceCanvas(stats_text_rows.getRowCanvas(i));
      stats_text[i].setTextColor(VolucrisGUIDefs::CONTENT_COMPONENT_TEXT_COLOR);
      stats_text[i].setFont(&myWindowFont);
      stats_text[i].setText(STATS_TEXT[i]);
      stats_text[i].setAlignment(GUIALIGN_LEFT);
      elements.insert(&stats_text[i]);

      stats_value_text[i].setSourceCanvas(stats_value_rows.getRowCanvas(i));
      stats_value_text[i].setTextColor(VolucrisGUIDefs::CONTENT_COMPONENT_TEXT_COLOR);
      stats_value_text[i].setFont(&myWindowFont);
      stats_value_text[i].setAlignment(GUIALIGN_RIGHT);
      elements.insert(&stats_value_text[i]);
    }
  }

  for (GUIElementSet::iterator i = elements.begin(); i != elements.end(); ++i) {
    GUIElement* element = *i;
    gui_primary_layer_.registerElement(element);
  }

  components.associate(&gui_primary_layer_,
                       myMouseInputSource.getPrimarySentinel(),
                       myKeyboardInputSource.getPrimarySentinel());
  tooltip_box.create(&gui_primary_layer_, 10, &mouse_cursor_canvas_);
  tooltip_box.becomeOwner(this);


  bool update = true;
  unsigned int points_left = 25;
  unsigned int stat_points[5] = { 10, 10, 10, 10, 10 };
  unsigned int selected_gender = 0;
  unsigned int selected_race = 0;
  unsigned int selected_class = 0;

  myStateMachine.jumpState(VCS_DISCONNECT);
  myStateMachine.queueState(VCS_SHUTDOWN);

  GUIFont small_font(VolucrisGUIDefs::TITLE_FONT_TEXTURE_INDEX);
  if (APP_ERROR(!small_font.create(myD3DDevice, "Trebuchet MS", 12))("Couldn't create font")) return;
  gui_primary_layer_.setTexture(small_font.getTextureID(), small_font.getTexture());
  EvidyonGUITextLabel tooltip_text;
  tooltip_text.setFont(&small_font);


  // Loop until we get a reply
  while (windowsMessagePump()) {
    // Poll the connection to keep it alive
    {
      ENetEvent netEvent;
      if (enet_host_service(myENetClient, &netEvent, 0) > 0) {
        if (netEvent.type == ENET_EVENT_TYPE_DISCONNECT){
          myStateMachine.jumpState(VCS_CONNECT);
          break;
        }
        else if (netEvent.type == ENET_EVENT_TYPE_RECEIVE && netEvent.channelID == 0) {
          using namespace ::Evidyon::Network::Packet;
          Message msg;
          decodeMessageType(netEvent.packet, &msg);
          if (msg == NETMSG_CREATECHARACTER_SUCCEEDED) {
            myStateMachine.jumpState(VCS_LOGGED_INTO_ACCOUNT);
            break;
          } else if (msg == NETMSG_REQUESTACTIONRESULT) {
            next.enable();  // re-enable the create button
            RequestActionResult rar = RAR_INVALID;
            Decode::requestActionResult(netEvent.packet, &rar);
            if (rar == RAR_FAILED_NAMEALREADYEXISTS) {
              errors.setText("Name already in use! Please choose another");
            } else {
              errors.setText("Too many characters on your account.");
            }
          }
          enet_packet_destroy(netEvent.packet);
        }
      }
    }

    { // set the tooltip
      bool tooltipping = false;
      GUIPoint mouse_position = myMouseInputSource.getMousePosition();
      for (int i = 0; i < 5; ++i) {
        if (stats_text_rows.getRowCanvas(i)->getArea()->contains(mouse_position)) {
          tooltipping = true;
          if (tooltip_box.becomeOwner(stats_text_rows.getRowCanvas(i))) {
            GUISize size;
            const char* text;
            switch (i) {
              case 0: text = "Increases the power of weapon strikes\nand adds a small amount of HP"; break;
              case 1: text = "Higher agility makes you cast, hit\nand run more quickly"; break;
              case 2: text = "Affects your avatar's health"; break;
              case 3: text = "Adds damage to offensive magic\nand adds to magic"; break;
              case 4: text = "Controls your avatar's magic and\nboosts the power of defensive spells"; break;
            }
            tooltip_text.setText(text, &size);
            tooltip_box.setContentSize(size);
            tooltip_box.showContentAfter(0.20);
            tooltip_box.addContent(&tooltip_text);
          }
        }
      }

      for (int i = 0; i < myResources.avatars.raceArraySize; ++i) {
        if (races[i].getCanvas()->getArea()->contains(mouse_position)) {
          tooltipping = true;
          if (tooltip_box.becomeOwner(&races[i])) {
            GUISize size;
            tooltip_text.setText(myResources.avatars.raceArray[i].description, &size);
            tooltip_box.setContentSize(size);
            tooltip_box.showContentAfter(0.40);
            tooltip_box.addContent(&tooltip_text);
          }
        }
      }

      for (int i = 0; i < myResources.avatars.classArraySize; ++i) {
        if (classes[i].getCanvas()->getArea()->contains(mouse_position)) {
          tooltipping = true;
          if (tooltip_box.becomeOwner(&classes[i])) {
            GUISize size;
            tooltip_text.setText(myResources.avatars.classArray[i].description, &size);
            tooltip_box.setContentSize(size);
            tooltip_box.showContentAfter(0.40);
            tooltip_box.addContent(&tooltip_text);
          }
        }
      }

      if (!next.isEnabled() && next.getCanvas()->getArea()->contains(mouse_position)) {
        tooltipping = true;
        if (tooltip_box.becomeOwner(&next)) {
          const char* text = "Add stat points to enable this button";
          GUISize size;
          tooltip_text.setText(text, &size);
          tooltip_box.setContentSize(size);
          tooltip_box.showContentAfter(0.20);
          tooltip_box.addContent(&tooltip_text);
        }
      }

      if (!tooltipping) {
        tooltip_box.becomeOwner(this);
      }
    }

    // Poll for genders/race/class change
    for (int i = 0; i < 2; ++i) {
      if (genders[i].wasPushed()) {
        genders[i].resetPushed();
        selected_gender = i;
        update = true;
      }
    }
    for (int i = 0; i < myResources.avatars.raceArraySize; ++i) {
      if (races[i].wasPushed()) {
        races[i].resetPushed();
        selected_race = i;
        update = true;
      }
    }
    for (int i = 0; i < myResources.avatars.classArraySize; ++i) {
      if (classes[i].wasPushed()) {
        classes[i].resetPushed();
        selected_class = i;
        update = true;
      }
    }

    tooltip_box.update(dcxWin32Clock::getAccurateSystemTime());

    if (update) {
      update = false;
      char points_left_text[128];
      sprintf_s(points_left_text, sizeof(points_left_text), "Points Remaining: %lu", points_left);
      stat_points_left_text.setText(points_left_text);
      for (int i = 0; i < 5; ++i) {

        if (points_left < 25 && stat_points[i] > 10) {
          stats_minus[i].enable();
        } else {
          stats_minus[i].disable();
        }

        unsigned int stat_max_value = myResources.avatars.raceArray[selected_race].max_stat[i];
        if (points_left > 0 && stat_points[i] < stat_max_value) {
          stats_plus[i].enable();
        } else {
          stat_points[i] = min(stat_points[i], stat_max_value);
          stats_plus[i].disable();
        }

        char value_text[8];
        sprintf_s(value_text, sizeof(value_text), "%lu/%lu", stat_points[i], stat_max_value);
        stats_value_text[i].setText(value_text);
      }

      if (points_left > 0) {
        next.disable();
      } else {
        next.enable();
      }
    }

    // Go to the main logged-in menu
    if (back.wasPushed()) {
      myStateMachine.jumpState(VCS_LOGGED_INTO_ACCOUNT);
      break;
    }

    if (next.wasPushed()) {
      next.resetPushed();
      next.disable();

      // Send a message to make this character on the server
      using namespace ::Evidyon::Network::Packet::Client;
      ENetPacket* packet = Encode::createCharacter(
          name.getText(),
          selected_gender == Evidyon::AVATARGENDER_MALE,
          selected_race,
          selected_class,
          stat_points[0],
          stat_points[1],
          stat_points[2],
          stat_points[3],
          stat_points[4]);
      enet_peer_send(myENetServer, 0, packet);

    }

    // Check stat point assignments
    for (int i = 0; i < 5; ++i) {
      if (stats_plus[i].wasPushed()) {
        stats_plus[i].resetPushed();
        if (stat_points[i] < 25 && points_left > 0) {
          points_left--;
          stat_points[i]++;
          update = true;
        }
      }
      if (stats_minus[i].wasPushed()) {
        stats_minus[i].resetPushed();
        if (stat_points[i] > 10 && points_left < 25) {
          points_left++;
          stat_points[i]--;
          update = true;
        }
      }

      // Assign the description
      char description_text[512];
      sprintf_s(description_text, sizeof(description_text), "%s %s %s",
                GENDERS_TEXT[selected_gender],
                myResources.avatars.raceArray[selected_race].name.c_str(),
                myResources.avatars.classArray[selected_class].name.c_str());
      description.setText(description_text);
    }

    if (allowFrameToBeRendered(dcxWin32Clock::getAccurateSystemTime()))
    {
        // Clear the backbuffer and the zbuffer
        myD3DDevice->Clear(0, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(0,0,0), 1.0f, 0);

        // Begin the scene
        if (SUCCEEDED(myD3DDevice->BeginScene())) {
            renderLoginBackground();
            global_rendering_manager_.resetCurrentTexture();
            gui_primary_layer_.render();
            myD3DDevice->EndScene();
        }

        // Present the backbuffer contents to the display
        CONFIRM(SUCCEEDED(myD3DDevice->Present(NULL, NULL, NULL, NULL))) else
        {
            // Wait for the device to come back, then return here
            myStateMachine.branchState(VCS_LOST_D3DDEVICE);
            break;
        }
    }
  }


  gui_primary_layer_.removeTexture(small_font.getTextureID());
  for (GUIElementSet::iterator i = elements.begin(); i != elements.end(); ++i) {
    GUIElement* element = *i;
    gui_primary_layer_.unregisterElement(element);
  }

  tooltip_box.destroy();
  components.clear();

  small_font.destroy();
}
