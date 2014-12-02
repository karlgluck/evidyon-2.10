#include "statisticslogger.h"
#include "time.h"
#include <direct.h>
#include <time.h>

#include "../shared/server-editor/evidyon_gameplay_design.h"

namespace Evidyon {

StatisticsLogger* global_statistics_logger = NULL;



bool StatisticsLogger::create() {

  // Move the existing stats file to a backup file name
  std::string stats_file_name = "statistics.txt";
  {
    // Generate the file name to which to move the database
    char file[256], mask[256];
    _mkdir("./statistics/");
    sprintf_s(mask, sizeof(mask), "./statistics/%%s.txt");

    // Find the current time
    tm * loc;
    time_t ltime;
    time(&ltime);
  	loc = localtime(&ltime);

    // Print the date and time
    char dateTime[32];
	    asctime_s(dateTime, sizeof(dateTime), loc);
    dateTime[strlen(dateTime)-1] = '\0';

    // Create the file
    sprintf_s(file, sizeof(file), mask, dateTime);

    // Remove invalid characters
    for (int i = 0; file[i] != '\0'; ++i)
      if (file[i] == ':')
        file[i] = '\'';

    // move the current stats file
    MoveFile(stats_file_name.c_str(), file);
  }

  if (!writer_.create(stats_file_name.c_str(), false)) return false;
  global_statistics_logger = this;
  return true;
}


void StatisticsLogger::destroy() {
  writer_.destroy();
  global_statistics_logger = NULL;
}

void StatisticsLogger::createDesignReport() {

}


void StatisticsLogger::characterLoggedIn(unsigned int account_number, unsigned int character_id, const char* name) {
  log("CharacterEnteredWorld\t%lu\t%lu\t%s\n", account_number, character_id, name);
}


void StatisticsLogger::characterLoggedOut(unsigned int account_number, unsigned int character_id, const char* name) {
  log("CharacterLeftWorld\t%lu\t%lu\t%s\n", account_number, character_id, name);
}


void StatisticsLogger::characterGainedLevel(unsigned int character_id, const char* name, int level) {
  log("LevelUp\t%lu\t%s\t%i\n", character_id, name, level);
}

void StatisticsLogger::characterCreated(unsigned int account_number, const char* name) {
  log("CreateCharacter\t%lu\t%s", account_number, name);
}


void StatisticsLogger::log(const char* mask, ...) {
  if (global_statistics_logger == NULL) return;

  char message_buffer[2048];

  va_list args;
  va_start(args, mask);
  int chars_written = _vsnprintf_s(message_buffer,
                                   sizeof(message_buffer),
                                   sizeof(message_buffer),
                                   mask,
                                   args);
  message_buffer[sizeof(message_buffer)-1] = '\0';
  va_end(args);

  global_statistics_logger->writer_.write(message_buffer,
                                          chars_written+1);
}




}