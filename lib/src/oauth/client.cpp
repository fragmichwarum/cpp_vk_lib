#include "oauth/client.hpp"

#include "exception/error-inl.hpp"
#include "methods/utility/constructor.hpp"
#include "simdjson.h"

vk::oauth::client::client(std::string_view username_, std::string_view password_, vk::oauth::target_client client_type_)
  : m_client_type(client_type_)
  , m_username(username_.data())
  , m_password(password_.data())
  , m_target_client_secret()
  , m_target_client_id()
  , m_pulled_token()
  , m_pulled_user_id(0)
  , m_parser(std::make_shared<simdjson::dom::parser>())
{
    switch (m_client_type)
    {
        case target_client::android:
            m_target_client_id = m_android_app_client_id;
            m_target_client_secret = m_android_app_client_secret;
            break;
        case target_client::iphone:
            m_target_client_id = m_iphone_app_client_id;
            m_target_client_secret = m_iphone_app_client_secret;
            break;
        case target_client::windows:
            m_target_client_id = m_windows_app_client_id;
            m_target_client_secret = m_windows_app_client_secret;
            break;
    }
    pull();
}

vk::oauth::client::~client() = default;

static bool error_returned(const simdjson::dom::object& response, std::string_view error_desc)
{
    return response.begin().key() == "error" && response["error"].get_string().take_value() == error_desc;
}

void vk::oauth::client::pull()
{
    method::constructor<method::method_parameter::do_not_use_api_link> raw_method_util;

    raw_method_util
        .method(std::string(m_oauth_link) + "token?")
        .param("grant_type", "password")
        .param("client_id", std::to_string(m_target_client_id))
        .param("client_secret", m_target_client_secret)
        .param("username", m_username.data())
        .param("password", m_password.data());

    simdjson::dom::object response = m_parser->parse(raw_method_util.execute());

    if (error_returned(response, "invalid_client") || error_returned(response, "invalid_request") ||
        error_returned(response, "invalid_grant"))
    {
        throw exception::access_error(-1, response["error_description"].get_c_str().take_value());
    }

    m_pulled_token = response["access_token"].get_c_str().take_value();
    m_pulled_user_id = response["user_id"].get_int64();
}

std::string vk::oauth::client::token() const noexcept
{
    return m_pulled_token;
}

std::int64_t vk::oauth::client::user_id() const noexcept
{
    return m_pulled_user_id;
}
