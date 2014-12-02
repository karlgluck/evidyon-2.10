#ifndef __RANDF_H__
#define __RANDF_H__

// Generates a random number [0.0, 1.0)
float randf();

// Generates a random number [0.0, f)
float randf(float max);

// Generates a random number [-f, f]
float randfSymmetric(float magnitude);

// Generates a random number [0, 2 pi)
float randfAngleRadians();

template <const size_t Entries> class RandSymmetricFloatTable {
public:
  void randomize() {
  for (int i = 0; i < Entries; ++i) {
    values_[i] = randfSymmetric(1.0f);
  }
  last_returned_ = 0;
  }

  // Gets the next entry in the float table.  This is theoretically much faster
  // than generating a truly random number each time a random value is needed,
  // and can be safely used in a variety of situations.
  inline float next() {
  last_returned_ = (last_returned_ + 1)%Entries;
  return values_[last_returned_];
  }

private:
  int last_returned_;
  float values_[Entries];
};


#endif