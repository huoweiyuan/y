#include "log.h"
#include <vector>

using namespace y::lib;
using namespace std;
Log::log_level Log::static_log_level = log_level::DEBUG;
vector<char> Log::static_log;

void Log::set_default_config(log_level level, unsigned int logLen)
{
  static_log_level = level;
  static_log.resize(logLen);
}

string Log::log_level_str(log_level level)
{
  switch (level) {
    case log_level::DEBUG: {
      return "DEBUG";
    }
    case log_level::INFO: {
      return "INFO";
    }
    case log_level::WARN: {
      return "WARN";
    }
    case log_level::ERROR: {
      return "ERROR";
    }
      // case LogLevel::CRITICAL: {
    default: {
      return "CRITICAL";
    }
  }
}

int Log::write(log_level level, const char *fileName, unsigned int line,
                     const char *funName, int fd, const char *format, ...)
{
  if (level < static_log_level) return 0;
  if (static_log.size() == 0) {
    static_log.resize(4096);
  }

  struct tm tmInfo;
  time_t now = time(nullptr);
  localtime_r(&now, &tmInfo);
  unsigned int len = snprintf(
      &static_log[0], static_log.size() - 1,
      "[%d-%d-%d %d:%d:%d][%lu][%s][%s][%s:%u]:", tmInfo.tm_year + 1900,
      tmInfo.tm_mon + 1, tmInfo.tm_mday, tmInfo.tm_hour, tmInfo.tm_min,
      tmInfo.tm_sec, (unsigned long)pthread_self(), log_level_str(level).c_str(),
      funName, fileName, line);

  va_list args;
  va_start(args, format);
  len += vsnprintf(&static_log[0] + len, static_log.size() - len - 1, format, args);
  va_end(args);
  if (len >= static_log.size() - 2)
  {
    static_log[static_log.size() - 2] = '\n';
    static_log[static_log.size() - 1] = '\0';
    len = static_log.size();
  }
  else
  {
    static_log[len] = '\n';
    static_log[len + 1] = '\0';
    len += 1;
  }
  ::write(fd, &static_log[0], len);
  return 0;
}