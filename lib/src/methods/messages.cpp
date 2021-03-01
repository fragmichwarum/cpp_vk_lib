#include "simdjson.h"

#include "keyboard/layout.hpp"
#include "methods/messages.hpp"
#include "method_utils/message_constructor.hpp"
#include "processing/process_error.hpp"


vk::method::messages::messages()
  : parser(std::make_unique<simdjson::dom::parser>())
{ }

vk::method::messages::~messages() = default;

void vk::method::messages::send(
    std::int64_t peer_id,
    std::string_view text,
    attachment::attachments_t&& list
) {
  message_constructor constructor;
  constructor.append({
    "peer_id", std::to_string(peer_id)
  });
  constructor.append({
    "message", text.data()
  });
  constructor.append_attachments(std::move(list));
  method_util.call("messages.send", method_util.group_args(constructor.consume_map()));
}

void vk::method::messages::send(
    std::int64_t peer_id,
    std::string_view text,
    std::map<std::string, std::string>&& raw_parameters
) {
  message_constructor constructor;
  constructor.append({
    "peer_id", std::to_string(peer_id)
  });
  constructor.append({
    "message", text.data()
  });
  constructor.append_map(
    std::move(raw_parameters)
  );
  method_util.call("messages.send", method_util.group_args(constructor.consume_map()));
}

void vk::method::messages::send(
    int64_t peer_id,
    std::string_view text,
    const vk::keyboard::layout& layout
) {
  message_constructor constructor;
  constructor.append({
    "peer_id", std::to_string(peer_id)
  });
  constructor.append({
    "message", text.data()
  });
  constructor.append({
    "keyboard", layout.serialize()
  });
  method_util.call("messages.send", method_util.group_args(constructor.consume_map()));
}

void vk::method::messages::send(std::int64_t peer_id, std::string_view text) {
  message_constructor constructor;
  constructor.append({
    "peer_id", std::to_string(peer_id)
  });
  constructor.append({
    "message", text.data()
  });
  method_util.call("messages.send", method_util.group_args(constructor.consume_map()));
}

void vk::method::messages::remove_chat_user(std::int64_t chat_id, std::int64_t user_id) {
  simdjson::dom::object response =
    method_util.call_and_parse("messages.removeChatUser", method_util.group_args({
      {"chat_id",    std::to_string(chat_id)},
      {"user_id",    std::to_string(user_id)},
      {"random_id",  "0"}
    }));

  if (response.begin().key() == "response" && response["response"].get_int64() == 1)
    return;

  if (response.begin().key() == "error") {
    processing::process_error("messages", exception::access_error(
      response["error"]["error_code"].get_int64(), response["error"]["error_msg"].get_c_str()
    ));
  }
}

void vk::method::messages::edit_chat(std::int64_t chat_id, std::string_view new_title) {
  method_util.call("messages.editChat", method_util.group_args({
    {"chat_id",     std::to_string(chat_id - method_util.chat_id_constant)},
    {"title",       new_title.data()},
    {"random_id",   "0"}
  }));
}

void vk::method::messages::delete_chat_photo(int64_t chat_id, int64_t group_id) {
  simdjson::dom::object response =
    method_util.call_and_parse("messages.deleteChatPhoto", method_util.group_args({
      {"chat_id",  std::to_string(chat_id - method_util.chat_id_constant)},
      {"group_id", std::to_string(group_id)}})
    );

  if (response.begin().key() == "error") {
    processing::process_error("messages", exception::access_error(
      response["error"]["error_code"].get_int64(), response["error"]["error_msg"].get_c_str()
    ));
  }
}

void vk::method::messages::pin(int64_t peer_id, int64_t message_id, std::int64_t conversation_message_id) {
  simdjson::dom::object response =
    method_util.call_and_parse("messages.pin", method_util.group_args({
      {"peer_id",                   std::to_string(peer_id)},
      {"message_id",                std::to_string(message_id)},
      {"conversation_message_id",   std::to_string(conversation_message_id)}
    }));

  if (response.begin().key() == "error") {
    processing::process_error("messages", exception::access_error(
      response["error"]["error_code"].get_int64(), response["error"]["error_msg"].get_c_str()
    ));
  }
}

void vk::method::messages::set_chat_photo(std::string_view filename, std::string_view raw_server) {
  simdjson::dom::object response =
    common_document.common_upload(filename, raw_server, "file");
  method_util.call("messages.setChatPhoto", method_util.group_args({
    {"file", response["response"].get_c_str().take_value()}
  }));
}

vk::conversation_member_list vk::method::messages::get_conversation_members(int64_t peer_id) {
  simdjson::dom::object response =
    method_util.call_and_parse(
      "messages.getConversationMembers",
      method_util.group_args({{"peer_id", std::to_string(peer_id)}})
    );

  if (response.begin().key() == "error") {
    processing::process_error("messages", exception::access_error(
      response["error"]["error_code"].get_int64(), response["error"]["error_msg"].get_c_str()
    ));
  }

  conversation_member_list members;
  for (auto&& profile : response["response"]["profiles"].get_array()) {
    members.push_back({
      profile["first_name"].get_string().take_value().data(),
      profile["last_name"].get_string().take_value().data(),
      profile["id"].get_int64(),
     (profile["online"].get_int64() == 1) ? true : false
    });
  }

  return members;
}
