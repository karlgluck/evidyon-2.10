//------------------------------------------------------------------------------------------------
// Copyright (c) 2007-2008 Unseen Studios.   All rights reserved.
//------------------------------------------------------------------------------------------------
#ifndef __VOLUCRISGUIGLOBALCHATDISPLAY_H__
#define __VOLUCRISGUIGLOBALCHATDISPLAY_H__

#include "volucrisguilist.h"

class VolucrisGUIGlobalChatDisplay : public VolucrisGUIList {
  static const int MAXIMUM_LINES = 8;
  static const int DISPLAY_DURATION_MILLIS = 6000;

  class GlobalChatLine : public VolucrisGUIList::VolucrisListEntry {
  public:
    GlobalChatLine();

    void initialize(GUIFont* font);
    void set(D3DCOLOR color, const std::string& text);
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
  VolucrisGUIGlobalChatDisplay();

  bool create(GUIFont* text_font, LPDIRECT3DTEXTURE9 gui_texture,
              GUICanvas* screen_canvas);
  void destroy();

  void show(GUILayer* layer);
  inline void hide() { show(NULL); }
  inline bool visible() { return layer_ != NULL; }

  void addSystemText(const std::string& system_text);

  // these methods will split the text into (at most) 2 lines
  void addPartyTextLine(const std::string& speaker_name, const std::string& text);

  void update(double time);

protected:
  void addLine(const std::string& line_text, int line_split_character_index, D3DCOLOR color);
  void ensureSpaceForNewLines(int number); // make sure to call refresh() after this method
  virtual unsigned int getNumberOfLines() const;
  virtual ListEntry* getListEntry(unsigned int index);
  inline int getNextAvailableLine() { return (first_line_index_ + num_lines_used_)%MAXIMUM_LINES; }

private:
  void zero();

private:
  GUICanvas background_canvas_;
  GUILayer* layer_;
  GlobalChatLine lines_[MAXIMUM_LINES];
  double line_expiration_times_[MAXIMUM_LINES];
  int first_line_index_;
  int num_lines_used_;
  double last_update_time_;
};

#endif