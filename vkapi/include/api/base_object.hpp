#pragma once

#include <string>
#include <memory>

#include "vkapi/include/attachment/attachment.hpp"

#define vk_always_inline inline __attribute__((always_inline))

namespace vk::lib       { class network; }
namespace simdjson::dom { class parser; }

namespace vk
{
class base_object
{
protected:
  explicit base_object() noexcept;
  virtual ~base_object();
  std::string append_url(std::string_view method);

  std::string access_token;
  std::string user_token;
  std::string api_v;
  std::string group_id;

  std::unique_ptr<lib::network> network;
  std::unique_ptr<simdjson::dom::parser> parser;
};
} // namespace vk
