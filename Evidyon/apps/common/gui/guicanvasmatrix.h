//-----------------------------------------------------------------------------
// Copyright (c) 2007-2008 Unseen Studios.   All rights reserved.
//-----------------------------------------------------------------------------
#ifndef __GUICANVASMATRIX_H__
#define __GUICANVASMATRIX_H__



template <size_t R, size_t C> class GUICanvasMatrix {
public:
  GUICanvasMatrix();
  void absoluteWidth(int width);
  void absoluteHeight(int height);
  GUICanvas* getCellFromPoint(GUIPoint point);

  inline void setSourceCanvas(GUICanvas* canvas) { matrix_.setSourceCanvas(canvas); }
  inline void alignXLeft(int x) { matrix_.alignXLeft(x); }
  inline void alignXCenter(int x) { matrix_.alignXCenter(x); }
  inline void alignXRight(int x) { matrix_.alignXRight(x); }
  inline void alignYTop(int y) { matrix_.alignYTop(y); }
  inline void alignYCenter(int y) { matrix_.alignYCenter(y); }
  inline void alignYBottom(int y) { matrix_.alignYBottom(y); }
  inline void absoluteDepth(float d) { matrix_.absoluteDepth(d); }
  inline void relativeDepth(float d) { matrix_.relativeDepth(d); }
  inline GUICanvas* getCellCanvas(int r, int c) { if (r<0||r>=R||c<0||c>=C) return 0; else return &cells_[r][c]; }
  inline void changedAlignment() { matrix_.changedAlignment(); }
  inline bool pointInside(GUIPoint point) { matrix_.pointInside(point); };
  inline const GUIRect* getArea() { return matrix_.getArea(); }
  inline float getDepth() { return matrix_.getDepth(); }

private:
  GUICanvas matrix_;
  GUICanvas cells_[R][C];
};





//-----------------------------------------------------------------------------
// Inline Implementation
//-----------------------------------------------------------------------------
template <size_t R, size_t C> GUICanvasMatrix<R,C>::GUICanvasMatrix() {
  matrix_.absoluteWidth(0);
  matrix_.absoluteHeight(0);
  for (size_t r = 0; r < R; ++r) {
  for (size_t c = 0; c < C; ++c) {
    cells_[r][c].setSourceCanvas(&matrix_);
    cells_[r][c].absoluteWidth(0);
    cells_[r][c].absoluteHeight(0);
    cells_[r][c].relativeDepth(0.0f);
  }
  }
}

template <size_t R, size_t C> void GUICanvasMatrix<R,C>::absoluteWidth(int width) {
  matrix_.absoluteWidth(width);
  int cell_width = width / C;
  for (size_t r = 0; r < R; ++r) {
  for (size_t c = 0; c < C; ++c) {
    cells_[r][c].alignXLeft(c * cell_width);
    cells_[r][c].absoluteWidth(cell_width);
  }
  }
}

template <size_t R, size_t C> void GUICanvasMatrix<R,C>::absoluteHeight(int height){
  matrix_.absoluteHeight(height);
  int cell_height = height / R;
  for (size_t r = 0; r < R; ++r) {
  for (size_t c = 0; c < C; ++c) {
    cells_[r][c].alignYTop(r * cell_height);
    cells_[r][c].absoluteHeight(cell_height);
  }
  }
}

template <size_t R, size_t C> GUICanvas* GUICanvasMatrix<R,C>::getCellFromPoint(GUIPoint point)
{
  const GUIRect* area = matrix_.getArea();
  point.x -= area->left; // make the point relative to this matrix's area
  point.y -= area->top;
  if (point.x < 0) return NULL;
  if (point.y < 0) return NULL;
  GUISize cell_size = area->calculateSize();
  cell_size.width /= C;
  cell_size.height /= R;
  return getCellCanvas(point.x/cell_size.width, point.y/cell_size.height);
}



#endif