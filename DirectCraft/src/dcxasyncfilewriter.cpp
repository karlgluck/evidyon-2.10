#include "../dcx/dcxasyncfilewriter.h"


#include "../dcx/dcxasyncdebugout.h"
#include "../dc/dcdebug.h"

#include <stdio.h>

namespace dcx {


dcxAsyncFileWriter::dcxAsyncFileWriter() {
  thread_handle_ = NULL;
  wait_event_ = NULL;
}


dcxAsyncFileWriter::~dcxAsyncFileWriter() {
  destroy();
}








DWORD WINAPI dcxAsyncFileWriterThread(void* writer) {
  dcxAsyncFileWriter* async_writer = (dcxAsyncFileWriter*)writer;
  async_writer->executeOutputThread();
  return S_OK;
}



bool dcxAsyncFileWriter::create(const char* output_file,
                                bool binary) {
  output_file_ = output_file;
  binary_ = binary;
  if (false == output_queue_.create()) return false;
  wait_event_ = CreateEvent(NULL, FALSE, FALSE, NULL);
  thread_handle_ = CreateThread(NULL,
                                0,
                                dcxAsyncFileWriterThread,
                                this,
                                THREAD_PRIORITY_NORMAL,
                                NULL);

  return true;
}



void dcxAsyncFileWriter::destroy() {
  if (thread_handle_) {
    int wait_counter = 0;
    do {
      ++wait_counter;
      SetEvent(wait_event_);
    } while (wait_counter < 5 &&
             WAIT_TIMEOUT == WaitForSingleObject(thread_handle_, 200));

    CloseHandle(wait_event_);
    CloseHandle(thread_handle_);
  }

  thread_handle_ = NULL;
  wait_event_ = NULL;
  output_queue_.destroy();
}




bool dcxAsyncFileWriter::seek(size_t location) {
  return false;
}




bool dcxAsyncFileWriter::move(int amount) {
  return false;
}



size_t dcxAsyncFileWriter::tell() {
  return (size_t)(-1);
}




bool dcxAsyncFileWriter::write(const void* buffer, size_t size) {
  char* data = new char[size+1];
  if (output_queue_.writeByAssignment(data)) {
    memcpy(data, buffer, sizeof(char)*size);
    data[size] = 0;
    SetEvent(wait_event_);
    return true;
  } else {
    delete[] data;
    return false;
  }
}



void dcxAsyncFileWriter::executeOutputThread() {
  FILE* file = NULL;
  if (0 != fopen_s(&file, output_file_.c_str(), binary_ ? "w+b" : "w+t")) return;

  while (true) {
    WaitForSingleObject(wait_event_, INFINITE);
    const char* output_text = NULL;
    while (output_queue_.readByAssignment(&output_text)) {
      OutputDebugString(output_text);
      fwrite(output_text,
             sizeof(char),
             strlen(output_text),
             file);
      delete[] output_text;
    }
    fflush(file);

    // if there were nothing in the buffer, exit
    if (!output_text) break;
  }

  fclose(file);
}



}