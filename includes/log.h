#ifndef Y_LOG_H
#define Y_LOG_H

#include <unistd.h>
#include <vector>

namespace y {
namespace lib {

class Log {
public:
  Log() = default;
  virtual ~Log() = default;

  enum class log_level { DEBUG, INFO, WARN, ERROR, CRITICAL };
  static log_level static_log_level;
  static std::vector<char> static_log;

public:
  static int write(log_level level, const char *fileName, unsigned int line,
                   const char *funName, int fd, const char *format, ...);
  static void set_default_config(log_level level, unsigned int logLen);
  static std::string log_level_str(log_level level);
};

#define LOG_DEBUG(fmt, args...)                                                \
  y::lib::Log::write(y::lib::Log::log_level::DEBUG, __FILE__, __LINE__,        \
                     __FUNCTION__, STDOUT_FILENO, fmt, ##args)
#define LOG_INFO(fmt, args...)                                                 \
  y::lib::Log::write(y::lib::Log::log_level::INFO, __FILE__, __LINE__,         \
                     __FUNCTION__, STDOUT_FILENO, fmt, ##args)
#define LOG_WARN(fmt, args...)                                                 \
  y::lib::Log::write(y::lib::Log::log_level::WARN, __FILE__, __LINE__,         \
                     __FUNCTION__, STDOUT_FILENO, fmt, ##args)
#define LOG_ERROR(fmt, args...)                                                \
  y::lib::Log::write(y::lib::Log::log_level::ERROR, __FILE__, __LINE__,        \
                     __FUNCTION__, STDOUT_FILENO, fmt, ##args)
#define LOG_CRITICAL(fmt, args...)                                             \
  y::lib::Log::write(y::lib::Log::log_level::CRITICAL, __FILE__, __LINE__,     \
                     __FUNCTION__, STDOUT_FILENO, fmt, ##args)
} // namespace lib
} // namespace y

#endif // Y_LOG_H