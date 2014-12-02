//------------------------------------------------------------------------------------------------
// File:    dcxinifilewriter.h
//
// Desc:    Provides a simple interface for writing a .ini settings file
//
// Copyright (c) 2007-2008 Unseen Studios.   All rights reserved.
//------------------------------------------------------------------------------------------------
#ifndef __DCXINIFILEWRITER_H__
#define __DCXINIFILEWRITER_H__


#include <map>


namespace dc {
class dcStreamOut;
}

namespace dcx {

class dcxIniFileReader;

/**
 * Provides a simple interface for writing a standard INI file
 */
class dcxIniFileWriter {
  // Map of Section Name -> (Map of Name->Parameter)
  typedef std::map<std::string,std::map<std::string,std::string>> Data;

public:

  // Interprets data from the file reader
  void scan(dcx::dcxIniFileReader* reader);

  // Changes or adds values to be written
  void setSetting(const std::string& section, const std::string& setting, int value);
  void setSetting(const std::string& section, const std::string& setting, float value);
  void setSetting(const std::string& section, const std::string& setting, const std::string& value);

  /**
   * Scans a source file that contains an INI that provides settings information.  This
   * method clears any information that has been previously obtained using read().  This
   * method returns 'false' only if the file doesn't exist.
   * @param file The path to the source file
   */
  bool write(dc::dcStreamOut* stream);

  bool write(const char* file_name);

private:
  Data data_;
};

}



#endif
