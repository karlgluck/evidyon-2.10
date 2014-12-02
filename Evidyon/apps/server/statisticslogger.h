#ifndef __STATISTICSLOGGER_H__
#define __STATISTICSLOGGER_H__


#include <dcx/asyncfilewriter>


namespace Evidyon {


class StatisticsLogger {
public:
  bool create();
  void destroy();

  // Prints out a full report containing information
  // helpful for analyzing the current design parameters
  static void createDesignReport();

public:
  static void characterLoggedIn(unsigned int account_number, unsigned int character_id, const char* name);
  static void characterLoggedOut(unsigned int account_number, unsigned int character_id, const char* name);
  static void characterGainedLevel(unsigned int character_id, const char* name, int level);
  static void characterCreated(unsigned int account_number, const char* name);

private:
  static void log(const char* message, ...);

private:
  dcx::dcxAsyncFileWriter writer_;
};


}



#endif