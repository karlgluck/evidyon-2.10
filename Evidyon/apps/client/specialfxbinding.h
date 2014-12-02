#ifndef __SPECIALFXBINDING_H__
#define __SPECIALFXBINDING_H__

namespace Evidyon {

class SpecialFXBinding {
public:
  SpecialFXBinding();
  virtual void update(double time, double time_since_last_update) = 0;
  virtual void get(float* x, float* y, float* z) = 0;

public:
  inline void setExpired() { expired_ = true; }
  inline bool getExpired() const { return expired_; }
  inline void addReference() { ++references_; }
  inline void removeReference() { --references_; }
  inline bool hasNoReferences() const { return references_ <= 0; }

private:
  bool expired_;
  int references_;
};

}


#endif