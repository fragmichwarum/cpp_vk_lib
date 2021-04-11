#include "oauth/client.hpp"

#include "exception/error_processor.hpp"
#include "simdjson.h"

vk::oauth::client::client(std::string_view username_, std::string_view password_, vk::oauth::target_client client_type_)
  : client_type(client_type_)
  , username(username_.data())
  , password(password_.data())
  , target_client_secret()
  , target_client_id()
  , pulled_token()
  , pulled_user_id(0)
  , parser(std::make_shared<simdjson::dom::parser>())
  , net_client() {
  switch (client_type) {
    case target_client::android:
      target_client_id = android_app_client_id;
      target_client_secret = android_app_client_secret;
      break;
    case target_client::iphone:
      target_client_id = iphone_app_client_id;
      target_client_secret = iphone_app_client_secret;
      break;
    case target_client::windows:
      target_client_id = windows_app_client_id;
      target_client_secret = windows_app_client_secret;
      break;
  }
  pull();
}

vk::oauth::client::~client() = default;

static bool error_returned(const simdjson::dom::object& response, std::string_view error_desc) {
  return response.begin().key() == "error" && response["error"].get_string().take_value() == error_desc;
}

void vk::oauth::client::pull() {
  simdjson::dom::object response = parser->parse(net_client.request(
      std::string(oauth_link) + "token?",
      {{"grant_type", "password"},
       {"client_id", std::to_string(target_client_id)},
       {"client_secret", target_client_secret},
       {"username", username.data()},
       {"password", password.data()}}));

  if (error_returned(response, "invalid_client") || error_returned(response, "invalid_request") ||
      error_returned(response, "invalid_grant")) {
    processing::log_and_throw<exception::access_error>("oauth", response["error_description"].get_c_str().take_value());
  }

  pulled_token = response["access_token"].get_c_str().take_value();
  pulled_user_id = response["user_id"].get_int64();
}

std::string vk::oauth::client::token() const noexcept {
  return pulled_token;
}

std::int64_t vk::oauth::client::user_id() const noexcept {
  return pulled_user_id;
}
