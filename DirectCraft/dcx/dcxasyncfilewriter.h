#ifndef __DCXASYNCFILEWRITER_H__
#define __DCXASYNCFILEWRITER_H__

#include "../dc/dcstream.h"
#include <windows.h>
#include "dcxasynchronoussisothreaddatacontroller.h"

namespace dcx {


class dcxAsyncFileWriter : public dc::dcStreamOut {
public:
  dcxAsyncFileWriter();
  virtual ~dcxAsyncFileWriter();
  bool create(const char* output_file, bool binary);
  void destroy();
  virtual bool seek(size_t location);
  virtual bool move(int amount);
  virtual size_t tell();
  virtual bool write(const void * buffer, size_t size );

public:
  void executeOutputThread();

private:
  dcx::dcxAsynchronousSISOThreadDataController<const char*, 16> output_queue_;
  HANDLE thread_handle_;
  HANDLE wait_event_;
  std::string output_file_;
  bool binary_;
};


}


#endif