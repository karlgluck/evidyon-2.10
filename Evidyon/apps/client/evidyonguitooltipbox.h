#ifndef __EVIDYONGUITOOLTIPBOX_H__
#define __EVIDYONGUITOOLTIPBOX_H__

#include "../common/gui/guigenericcanvas.h"
#include "../common/gui/guicanvas.h"
#include "../common/gui/guicomponentset.h"
#include "../common/gui/guiborderedfillelement.h"

namespace Evidyon {

// TODO: implement a callback in GUIComponentSet so that all of the
//       source canvases get reset when this class is cleared!

// this box follows the mouse cursor, can be resized and will not cross
// the edge of the screen by more than a certain amount
class EvidyonGUITooltipBox {
  class Canvas : public GUIGenericCanvas {
  public:
    Canvas(EvidyonGUITooltipBox* owner);
    void setContentSize(GUISize size);
    void setSourceCanvas(GUIGenericCanvas* canvas);
  private:
    virtual void realign();
  private:
    GUISize content_size_;
    EvidyonGUITooltipBox* owner_;
  };


public:
  EvidyonGUITooltipBox();
  ~EvidyonGUITooltipBox();
  void create(GUILayer* primary_layer,
              int pixels_tolerance,
              GUIGenericCanvas* mouse_cursor_canvas);
  void destroy();

  // If this becomeOwner() returns 'true', the calling method's unique
  // pointer is now used as the reference for ownership of this tooltip.
  // All components have been cleared.
  // Ownership is preemptive: if it is requested, it is taken from
  // another owner.  The only time this method returns 'false' is if
  // the unique pointer is identical to the current owner--i.e., the
  // owner is trying to preempt itself.
  // A good unique_ptr for most apps is simply '(void*)this'
  bool becomeOwner(void* unique_ptr);

  // Resets the owner, hides this tooltip, and clears the content as long
  // as the unique_ptr specified is the current owner (so that preempted
  // owners don't accidentally overwrite others' data)
  void releaseOwnership(void* unique_ptr);

  bool hasOwnership(void* unique_ptr) const;

  void setContentSize(GUISize size);
  void addContent(GUIComponent* component);
  void removeContent(GUIComponent* component);

  // Causes the tooltip to appear after a given amount of time.  This method
  // must be called after adding content before it will be displayed.
  void showContentAfter(double seconds);

  // Registers the tooltip's contents, if it has just become visible
  void update(double time);


private:
  void clearContent();

private:
  void* owner_;
  double last_update_time_;
  double next_visible_time_;
  GUILayer* primary_layer_;
  GUIGenericCanvas* mouse_cursor_canvas_;

  // The canvas of the tooltip box will not leave this area.  Based on the
  // primary layer's canvas size.
  GUICanvas boundaries_;

  // The canvas that defines the box
  Canvas canvas_;

  // Stuff inside the tooltip box
  GUIComponentSet components_;

  GUIBorderedFillElement background_;
};


}



#endif