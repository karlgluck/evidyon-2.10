#include "randf.h"
#include <stdlib.h>

float randf() {
  return rand()/(float)RAND_MAX;
}

float randfSymmetric(float magnitude) {
  return magnitude * (2.0f * (rand()/(float)RAND_MAX) - 1.0f);
}

float randfAngleRadians() {
  return 2.0f * 3.1415926535f * (rand()/(float)RAND_MAX);
}