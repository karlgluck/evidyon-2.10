#include "../dcx/dcxinifilewriter.h"
#include "../dcx/dcxinifilereader.h"
#include <dc/stream>
#include <dc/debug>
#include <dc/filestream>

namespace dcx {



  
//----[  scan  ]---------------------------------------------------------------
void dcxIniFileWriter::scan(dcx::dcxIniFileReader* reader) {
  dcxIniFileReader::SettingMap::iterator settings_end = reader->mySettings.end();
  for (dcxIniFileReader::SettingMap::iterator i = reader->mySettings.begin(); i != settings_end; ++i) {
    std::string section = i->first.first;
    const std::string& setting = i->first.second;
    const std::string& value = i->second;
    setSetting(section, setting, value);
  }
}



//----[  setSetting  ]---------------------------------------------------------
void dcxIniFileWriter::setSetting(const std::string& section, const std::string& setting, int value) {
  char buffer[64];
  sprintf_s(buffer, 64, "%i", value);
  setSetting(section, setting, buffer);
}


void dcxIniFileWriter::setSetting(const std::string& section, const std::string& setting, float value) {
  char buffer[64];
  sprintf_s(buffer, 64, "%f", value);
  setSetting(section, setting, buffer);
}

void dcxIniFileWriter::setSetting(const std::string& section, const std::string& setting, const std::string& value) {
  Data::iterator section_iter = data_.find(section);
  if (section_iter == data_.end()) {
    Data::_Pairib ib = data_.insert(Data::value_type(section, Data::mapped_type()));
    section_iter = ib.first;
  }
  Data::mapped_type& settings = section_iter->second;
  Data::mapped_type::iterator setting_iter = settings.find(setting);
  if (setting_iter == settings.end()) {
    settings.insert(Data::mapped_type::value_type(setting, value));
  } else {
    setting_iter->second.assign(value);
  }
}




//----[  write  ]--------------------------------------------------------------
bool dcxIniFileWriter::write(dc::dcStreamOut* stream) {
  char data[512];
  for (Data::iterator i = data_.begin(); i != data_.end(); ++i) {
    sprintf_s(data, "%s[%s]\n", (i == data_.begin() ? "" : "\n\n"), i->first.c_str());
    CONFIRM(stream->write(data, strlen(data))) else return false;
    for (Data::mapped_type::iterator j = i->second.begin(); j != i->second.end(); ++j) {
      sprintf_s(data, "%s=%s\n", j->first.c_str(), j->second.c_str());
      CONFIRM(stream->write(data, strlen(data))) else return false;
    }
  }

  return true;
}




//----[  write  ]--------------------------------------------------------------
bool dcxIniFileWriter::write(const char* file_name) {
  dc::dcFileStream file_stream;
  if (!file_stream.open(file_name, STREAM_OVERWRITE|STREAM_TEXT)) return false;
  bool return_value = write(&file_stream);
  file_stream.close();
  return return_value;
}

}