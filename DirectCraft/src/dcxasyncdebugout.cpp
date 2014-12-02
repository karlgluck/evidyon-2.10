#include "../dcx/dcxasyncdebugout.h"
#include "../dc/dcdebug.h"

namespace dcx {


dcxAsyncDebugOut::~dcxAsyncDebugOut() {
  releaseDebugStream();
}


bool dcxAsyncDebugOut::write(const void* buffer, size_t size ) {
  return dcxAsyncFileWriter::write(buffer, size);
}

bool dcxAsyncDebugOut::acquireDebugStream() {
  std::string output_filename = dc::dcDebug::global(__FILE__,__LINE__)->getLogFilename();
  output_filename = output_filename.substr(0, output_filename.find_last_of(".")) + " - async.txt";
  if (!create(output_filename.c_str(), false)) return false;
  dc::dcDebug::global(__FILE__,__LINE__)->changeOutputStream(this);
  return true;
}

void dcxAsyncDebugOut::releaseDebugStream() {
  dc::dcDebug::global(__FILE__,__LINE__)->changeOutputStream(NULL);
  destroy();
}



}