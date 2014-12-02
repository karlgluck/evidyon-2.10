#include "updatefile.h"
#include <dcx/dcxwin32internetstream.h>

namespace Evidyon {

bool DownloadUpdateFile(const char* webaddr, UpdateFile* file) {

  std::string update_file;
  {
    dcx::dcxWin32InternetStream stream;
    if (false == stream.open("EvidyonUpdateUploader", webaddr)) {
      return false;
    }
    static const int BUFFER_SIZE = 512;
    char buffer[BUFFER_SIZE+1];
    size_t bytes_read = 0;
    while (stream.scan(buffer, BUFFER_SIZE, &bytes_read) && !stream.end()) {
      buffer[bytes_read] = '\0';
      update_file.append(buffer);
    }
    stream.close();

    if (update_file.find(Evidyon::UPDATE_FILE_HEADER_TEXT) != 0) return false;
    update_file = update_file.substr(strlen(Evidyon::UPDATE_FILE_HEADER_TEXT));
  }

  // Find locations
  size_t major_end = update_file.find_first_of("\r\n");
  size_t minor_end = update_file.find_first_of("\r\n", major_end + 2);

  sscanf_s(update_file.substr(0, major_end).c_str(), "%lu", &file->major);
  sscanf_s(update_file.substr(major_end+2, minor_end-2).c_str(), "%lu", &file->minor);

  update_file = update_file.substr(minor_end+2);

  while (false == update_file.empty()) {
    size_t filename_end = update_file.find_first_of("\r\n");
    size_t file_size_end = update_file.find_first_of("\r\n", filename_end + 2);
    size_t md5_end = update_file.find_first_of("\r\n", file_size_end + 2);
    std::string filename = update_file.substr(0, filename_end);
    std::string file_size = update_file.substr(filename_end + 2, file_size_end - filename_end - 2);
    std::string md5 = update_file.substr(file_size_end+2, md5_end-file_size_end-2);
    update_file = update_file.substr(md5_end+2);
    {
      UpdateFileData file_data;
      file_data.md5 = md5;
      sscanf_s(file_size.c_str(), "%I64d", &file_data.file_bytes);
      file->files.insert(std::map<std::string,UpdateFileData>::value_type(filename, file_data));
    }
  }

  return true;
}

}