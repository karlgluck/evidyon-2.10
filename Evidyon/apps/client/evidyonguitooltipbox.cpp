#include "evidyonguitooltipbox.h"
#include "volucrisguidefs.h"
#include "../common/gui/guicomponent.h"
#include "../common/gui/guilayer.h"


namespace Evidyon {

EvidyonGUITooltipBox::Canvas::Canvas(EvidyonGUITooltipBox* owner) {
  owner_ = owner;
  content_size_.zero();
}


void EvidyonGUITooltipBox::Canvas::setContentSize(GUISize size) {
  content_size_ = size;
}

void EvidyonGUITooltipBox::Canvas::setSourceCanvas(GUIGenericCanvas* canvas) {
  changeSourceCanvas(canvas);
  changedAlignment();
}

void EvidyonGUITooltipBox::Canvas::realign() {
  getSourceCanvas();
  const GUIRect* mouse = owner_->mouse_cursor_canvas_->getArea();
  GUIPoint pt = { mouse->right + 5, mouse->bottom + 5 };
  area_.set(pt, content_size_);
  const GUIRect* boundaries = owner_->boundaries_.getArea();
  if (area_.right > boundaries->right) {
    pt.x = mouse->left - 5 - content_size_.width;
  }
  if (area_.bottom > boundaries->bottom) {
    pt.y = mouse->top - 5 - content_size_.height;
  }
  area_.set(pt, content_size_);
  depth_ = 0.1f;
}


EvidyonGUITooltipBox::EvidyonGUITooltipBox() : canvas_(this) {
  owner_ = NULL;
  last_update_time_ = 0.0;
  next_visible_time_ = -1.0;
  primary_layer_ = NULL;
  mouse_cursor_canvas_ = NULL;

  boundaries_.relativeWidth(0);
  boundaries_.relativeHeight(0);
  boundaries_.relativeDepth(0.0f);
  boundaries_.alignXLeft(0);
  boundaries_.alignYTop(0);

  background_.setSourceCanvas(&canvas_);
  background_.setTexture(VolucrisGUIDefs::GUI_TEXTURE_INDEX);
  background_.setSourceRegion(&VolucrisGUIDefs::WINDOW_BACKGROUND, 8, VolucrisGUIDefs::TEXTURE_SIZE);
}


EvidyonGUITooltipBox::~EvidyonGUITooltipBox() {
  destroy();
}


void EvidyonGUITooltipBox::create(GUILayer* primary_layer,
                                  int pixels_tolerance,
                                  GUIGenericCanvas* mouse_cursor_canvas) {
  primary_layer_ = primary_layer;
  canvas_.setSourceCanvas(mouse_cursor_canvas); 
  boundaries_.setSourceCanvas(primary_layer->getPrimaryCanvas());
  boundaries_.relativeWidth(pixels_tolerance);
  boundaries_.relativeHeight(pixels_tolerance);
  mouse_cursor_canvas_ = mouse_cursor_canvas;
}


void EvidyonGUITooltipBox::destroy() {
  clearContent();
}


bool EvidyonGUITooltipBox::becomeOwner(void* unique_ptr) {
  if (owner_ != unique_ptr) {
    clearContent();
    owner_ = unique_ptr;
    return true;
  } else {
    return false;
  }
}


void EvidyonGUITooltipBox::releaseOwnership(void* unique_ptr) {
  if (owner_ == unique_ptr) {
    clearContent();
    owner_ = NULL;
  }
}

bool EvidyonGUITooltipBox::hasOwnership(void* unique_ptr) const {
  return owner_ == unique_ptr;
}


void EvidyonGUITooltipBox::setContentSize(GUISize size) {
  canvas_.setContentSize(size);
}

void EvidyonGUITooltipBox::addContent(GUIComponent* component) {
  component->getCanvas()->setSourceCanvas(&canvas_);
  components_.add(component);
}


void EvidyonGUITooltipBox::removeContent(GUIComponent* component) {
  components_.remove(component);
  component->getCanvas()->setSourceCanvas(NULL);
}


void EvidyonGUITooltipBox::showContentAfter(double seconds) {
  next_visible_time_ = last_update_time_ + seconds;
}


void EvidyonGUITooltipBox::update(double time) {
  if (last_update_time_ <= next_visible_time_ &&
      time >= next_visible_time_) {
    primary_layer_->registerElement(&background_);
    components_.registerElements(primary_layer_);
    next_visible_time_ = -1.0; // reset (so this can't be done multiple times)
  }

  last_update_time_ = time;
}


void EvidyonGUITooltipBox::clearContent() {
  primary_layer_->unregisterElement(&background_);
  components_.clear();

  // this ordering makes update() not try to re-register components, so
  // the components never become visible!
  next_visible_time_ = -1.0;
}




}


