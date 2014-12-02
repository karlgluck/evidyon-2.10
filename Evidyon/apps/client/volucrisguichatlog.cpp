//------------------------------------------------------------------------------------------------
// File:    volucrisguichatlog.cpp
//
// Desc:    
//
// Copyright (c) 2007-2008 Unseen Studios.   All rights reserved.
//------------------------------------------------------------------------------------------------
#include "stdafx.h"





//------------------------------------------------------------------------------------------------
VolucrisGUIChatLog::ChatLogLine::ChatLogLine()
    : text_element_(this) {
  zero();
}


//------------------------------------------------------------------------------------------------
void VolucrisGUIChatLog::ChatLogLine::initialize(GUIFont* font) {
  text_element_.setFont(font);
  text_element_.setTextColor(0);
  text_element_.setAlignment(GUIALIGN_LEFT);
}


//------------------------------------------------------------------------------------------------
void VolucrisGUIChatLog::ChatLogLine::set(D3DCOLOR color, const std::string& text) {
  text_element_.setTextColor(color);
  text_element_.setText(text);
}


//------------------------------------------------------------------------------------------------
void VolucrisGUIChatLog::ChatLogLine::copy(const ChatLogLine& other) {
  text_element_.setTextColor(other.text_element_.getTextColor());
  text_element_.setText(other.text_element_.getText());
}



//------------------------------------------------------------------------------------------------
void VolucrisGUIChatLog::ChatLogLine::registerElements(GUILayer* layer) {
  text_element_.registerElements(layer);
}


//------------------------------------------------------------------------------------------------
void VolucrisGUIChatLog::ChatLogLine::unregisterElements(GUILayer* layer) {
  text_element_.unregisterElements(layer);
}


//------------------------------------------------------------------------------------------------
void VolucrisGUIChatLog::ChatLogLine::refresh() {
  text_element_.setClippingArea(getClippingArea());
}



//------------------------------------------------------------------------------------------------
void VolucrisGUIChatLog::ChatLogLine::zero() {
  text_element_.getCanvas()->alignXLeft(+10);
  text_element_.getCanvas()->alignYCenter(0);
}



//------------------------------------------------------------------------------------------------
VolucrisGUIChatLog::VolucrisGUIChatLog()
    : slider_(this) {
  zero();
}



//------------------------------------------------------------------------------------------------
bool VolucrisGUIChatLog::create(GUIFont* text_font,
                                GUIMouseSentinel* parent_sentinel) {
  destroy();

  list_body_canvas_.setSourceCanvas(&background_canvas_);
  list_body_canvas_.relativeDepth(-0.01f);
  list_body_canvas_.alignXCenter(0);
  list_body_canvas_.alignYCenter(0);
  list_body_canvas_.relativeWidth(-16);
  list_body_canvas_.relativeHeight(-16);


  lines_ = new ChatLogLine[MAXIMUM_CHAT_LOG_LINES];
  ASSERT(lines_) else return false;

  setLineHeight((int)(text_font->getLineHeightInPixels() * 1.0f));

  for (int i = 0; i < MAXIMUM_CHAT_LOG_LINES; ++i) {
    lines_[i].initialize(text_font);
  }

  parent_sentinel_ = parent_sentinel; // don't set into slider until the log is enabled
  slider_.setListCanvases(&list_body_canvas_);

  GUIRect area = { 64, 0, 95, 31 };
  background_.setTexture(VolucrisGUIDefs::GUI_TEXTURE_INDEX);
  background_.setColor(D3DCOLOR_ARGB(220,255,255,255));
  background_.setSourceRegion(&area, 8, VolucrisGUIDefs::TEXTURE_SIZE);
  background_.setSourceCanvas(&background_canvas_);

  refresh();

  return true;
}



//------------------------------------------------------------------------------------------------
void VolucrisGUIChatLog::destroy() {
  hide();
  slider_.removeFromMouseInputChain();
  layer_ = 0;

  if (lines_) {
    delete[] lines_;
    lines_ = 0;
  }

  zero();
}



//------------------------------------------------------------------------------------------------
void VolucrisGUIChatLog::show(GUILayer* layer) {
  if (layer_ != NULL) { // erase elements from the old layer
    unregisterElements(layer_);
    layer_->unregisterElement(&background_);
    slider_.unregisterElements(layer_);
    slider_.removeFromMouseInputChain();
  }
  if (layer != NULL) { // add to the new layer, if it exists
    registerElements(layer);
    layer->registerElement(&background_);
    slider_.registerElements(layer);
    slider_.setParentMouseSentinel(parent_sentinel_);
    slider_.syncToList();
  }
  layer_ = layer;
}




//------------------------------------------------------------------------------------------------
void VolucrisGUIChatLog::addSystemText(const std::string& system_text) {
  addLine(system_text, system_text.length()/2, SYSTEM_TEXT_COLOR);
}



//------------------------------------------------------------------------------------------------
void VolucrisGUIChatLog::addReceivedChatLine(const std::string& speaker_name, const std::string& text, bool party_chat) {
  std::string chat_line = speaker_name;
  chat_line.append(":  ");
  chat_line.append(text);
  addLine(chat_line, speaker_name.length() + 1, party_chat ? PARTY_TEXT_COLOR : CHAT_TEXT_COLOR);
}




//------------------------------------------------------------------------------------------------
void VolucrisGUIChatLog::addOwnChatLine(const std::string& text) {
    addLine(text, text.length()/2, OWN_TEXT_COLOR);
}




//------------------------------------------------------------------------------------------------
void VolucrisGUIChatLog::addLine(const std::string& text_line, int split_character_index, D3DCOLOR color) {
  bool scroll_to_end = (getScrollIndexPercent() > 0.95f);
  unsigned int first_line_index;
  int first_line_offset;
  getScrollLine(&first_line_index, &first_line_offset);

  ASSERT(lines_) else return;
  GUISize text_size;
  if (!lines_->getFont()->getTextExtent(text_line.c_str(), -1, &text_size)) return;
  GUISize target_size = myTargetCanvas.getArea()->calculateSize();
  if ((text_size.width) < target_size.width) { // add the whole text line
    ensureSpaceForNewLines(1);
    lines_[num_lines_used_].set(color, text_line);
    if (layer_) lines_[num_lines_used_].registerElements(layer_); // make sure the element is visible
    ++num_lines_used_;
  } else { // add the text line and character name on separate lines
    ensureSpaceForNewLines(2);

    lines_[num_lines_used_].set(color, text_line.substr(0,split_character_index)); // add the first text section
    if (layer_) lines_[num_lines_used_].registerElements(layer_);
    ++num_lines_used_;

    lines_[num_lines_used_].set(color, text_line.substr(split_character_index)); // add the second text section
    if (layer_) lines_[num_lines_used_].registerElements(layer_);
    ++num_lines_used_;
  }

  // update scrolling location
  if (scroll_to_end) {
    scrollToPercent(1.0f); // maintain view at end of list; will call refresh()
  } else {
    scrollToLine(first_line_index, first_line_offset); // keep current position; calls refresh()
  }
  slider_.syncToList();
}



//------------------------------------------------------------------------------------------------
void VolucrisGUIChatLog::ensureSpaceForNewLines(int number) {
  if (num_lines_used_ + number >= MAXIMUM_CHAT_LOG_LINES) {
    int lines_to_roll = MAXIMUM_CHAT_LOG_LINES/4 + number;
    CONFIRM(lines_to_roll < MAXIMUM_CHAT_LOG_LINES) else lines_to_roll = MAXIMUM_CHAT_LOG_LINES-1;
    int new_last_line_index = MAXIMUM_CHAT_LOG_LINES-lines_to_roll;
    for (int i = 0; i < new_last_line_index; ++i) {
      lines_[i].copy(lines_[i+lines_to_roll]);
    }
    std::string empty = "";
    for (int i = new_last_line_index; i < num_lines_used_; ++i) {
      lines_[i].set(0,empty);
    }
    num_lines_used_ -= lines_to_roll;
    ASSERT(num_lines_used_>0);
  }
}



//------------------------------------------------------------------------------------------------
unsigned int VolucrisGUIChatLog::getNumberOfLines() const {
  return num_lines_used_;
}



//------------------------------------------------------------------------------------------------
GUIList::ListEntry* VolucrisGUIChatLog::getListEntry(unsigned int index) {
  return (index < num_lines_used_) ? &lines_[index] : NULL;
}


//------------------------------------------------------------------------------------------------
void VolucrisGUIChatLog::zero() {
  layer_ = NULL;
  lines_ = NULL;
  num_lines_used_ = 0;
  parent_sentinel_ = NULL;
}