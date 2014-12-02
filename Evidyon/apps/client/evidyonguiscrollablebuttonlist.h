#ifndef __EVIDYONGUISCROLLABLEBUTTONLIST_H__
#define __EVIDYONGUISCROLLABLEBUTTONLIST_H__


#include "evidyonguiscrollablelist.h"
#include "volucrisguibutton.h"


namespace Evidyon {


class EvidyonGUIScrollableButtonList : public GUIComponent {
  class Button : public VolucrisGUIButton {
  public:
    Button(EvidyonGUIScrollableButtonList* owner, int id);
    int getID() const;

    // Interprets mouse over and click messages, but always
    // passes back up to VolucrisGUIButton.
    virtual void onStateChange(GUIButton::StateChange stateChange);

  private:
    int id_;
    EvidyonGUIScrollableButtonList* owner_;
  };

public:
  EvidyonGUIScrollableButtonList();
  virtual ~EvidyonGUIScrollableButtonList();

  // Changes how large the buttons are
  void setLineHeight(int pixels);

  // Either obtains a currently existing button or creates a new button
  // and returns that one.
  // The visual properties of the buttons (text and font) must be entirely
  // managed by the class using this one.
  VolucrisGUIButton* button(int id);

  // Erases all buttons.  Be sure to call unregisterElements before this
  // method!
  void clear();

  // Returns 'true' if the mouse is hovering over a button, and passes
  // the ID of that entry in the parameter.
  bool getMouseOverButton(int* id);

  // Returns 'true' if the mouse clicked a button, and passes
  // the ID of that entry in the parameter.  This method will
  // only return 'true' once per button click.
  bool getClickedButton(int* id);

public:
  virtual GUIMouseSentinel* getMouseSentinel();
  virtual GUIKeyboardSentinel* getKeyboardSentinel();
  virtual void registerElements(GUILayer* layer);
  virtual void unregisterElements(GUILayer* layer);

private:
  Button* mouse_over_button_;
  Button* mouse_clicked_button_;
  EvidyonGUIScrollableList list_;
};

}



#endif
