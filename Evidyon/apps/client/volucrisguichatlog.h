//------------------------------------------------------------------------------------------------
// File:    volucrisguichatlog.h
//
// Desc:    
//
// Copyright (c) 2007-2008 Unseen Studios.   All rights reserved.
//------------------------------------------------------------------------------------------------
#ifndef __VOLUCRISGUICHATLOG_H__
#define __VOLUCRISGUICHATLOG_H__


#include "volucrisguilist.h"

class VolucrisGUIChatLog : public VolucrisGUIList {
  class ChatLogLine : public VolucrisGUIList::VolucrisListEntry {
  public:
    ChatLogLine();

    void initialize(GUIFont* font);
    void set(D3DCOLOR color, const std::string& text);
    void copy(const ChatLogLine& other);
    inline GUIFont* getFont() { return text_element_.getFont(); }

    virtual void registerElements(GUILayer* layer);
    virtual void unregisterElements(GUILayer* layer);
    virtual void refresh();

  private:
    void zero();

  private:
    VolucrisGUIListEntryText text_element_;
  };

public:
  static const D3DCOLOR CHAT_TEXT_COLOR   = D3DCOLOR_XRGB(255,255,255);
  static const D3DCOLOR OWN_TEXT_COLOR    = D3DCOLOR_XRGB(192,64,64);
  static const D3DCOLOR PARTY_TEXT_COLOR  = D3DCOLOR_XRGB(193, 209, 182);
  static const D3DCOLOR SYSTEM_TEXT_COLOR = D3DCOLOR_XRGB(64,192,64);
  static const int MAXIMUM_CHAT_LOG_LINES = 512;

public:
  VolucrisGUIChatLog();

  bool create(GUIFont* text_font, GUIMouseSentinel* parent_sentinel);
  void destroy();

  GUICanvas* getBackgroundCanvas() { return &background_canvas_; }

  void show(GUILayer* layer);
  inline void hide() { show(NULL); }
  inline bool visible() { return layer_ != NULL; }

  void addSystemText(const std::string& system_text);

  // these methods will split the text into (at most) 2 lines
  void addReceivedChatLine(const std::string& speaker_name, const std::string& text, bool party_chat);
  void addOwnChatLine(const std::string& text);

protected:
  void addLine(const std::string& line_text, int line_split_character_index, D3DCOLOR color);
  void ensureSpaceForNewLines(int number); // make sure to call refresh() after this method
  virtual unsigned int getNumberOfLines() const;
  virtual ListEntry* getListEntry(unsigned int index);

private:
  void zero();

private:
  GUICanvas background_canvas_;
  GUIBorderedFillElement background_;
  GUICanvas list_body_canvas_;
  GUILayer* layer_;
  ListSlider slider_;
  ChatLogLine* lines_;
  int num_lines_used_;
  GUIMouseSentinel* parent_sentinel_;
};



#endif