#pragma once

#include <stdexcept>

namespace vk::lib
{
class common_error : public std::exception
{
public:
  common_error()                                = delete;
  common_error(const common_error&)             = delete;
  common_error(common_error&&)                  = delete;
  common_error& operator=(const common_error&)  = delete;
  common_error& operator=(common_error&&)       = delete;

  common_error(const char* file, long line, long id, const char* message)
    : file    (file)
    , msg     (message)
    , line    (std::to_string(line))
    , id      (std::to_string(id))
  { }

  const char* what() const noexcept override
  {
    static std::string what = file + ':' + line + " [vk.common_error." + id + "]: " + msg;
    return what.c_str();
  }

private:
  std::string file;
  std::string msg;
  std::string line;
  std::string id;
};
}
