#ifndef __UPDATEFILE_H__
#define __UPDATEFILE_H__

#include <string>
#include <map>


namespace Evidyon {

struct UpdateFileData {
  std::string md5;
  __int64 file_bytes;
};

struct UpdateFile {
  std::string update_text;
  unsigned long major, minor;
  std::map<std::string, UpdateFileData> files;
};

// This method contacts the update server, then interprets and downloads
// the current file.  If it is invalid, or the server could not be
// contacted, it returns 'false'.
bool DownloadUpdateFile(const char* webaddr, UpdateFile* file);

static const char* UPDATE_FILE_HEADER_TEXT = "EvidyonUpdateFile";


}

#endif