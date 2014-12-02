#ifndef __DCXASYNCDEBUG_H__
#define __DCXASYNCDEBUG_H__

#include "dcxasyncfilewriter.h"

namespace dcx {

class dcxAsyncDebugOut : public dcx::dcxAsyncFileWriter {
public:
  ~dcxAsyncDebugOut();
  virtual bool write(const void * buffer, size_t size );

public:
  bool acquireDebugStream();
  void releaseDebugStream();

};


}


#endif