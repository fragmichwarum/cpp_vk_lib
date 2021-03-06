#ifndef VK_MESSAGES_H
#define VK_MESSAGES_H

#include "document/common.hpp"
#include "methods/utility/constructor.hpp"

namespace vk {
namespace keyboard {
class layout;
}// namespace keyboard
}// namespace vk

namespace vk {
struct conversation_member
{
    std::string first_name;
    std::string last_name;
    std::int64_t id;
    bool online;
};

using conversation_member_list = std::vector<conversation_member>;

namespace method {
/*!
 * @brief The messages methods representation.
 *
 * Please, inherit this class to add new methods.
 */
class messages
{
public:
    explicit messages(bool disable_mentions_flag);
    messages() = delete;

    ~messages();

    static inline bool disable_mentions = true;
    static inline bool enable_mentions = false;

    void send(std::int64_t peer_id, std::string_view text) const;
    void send(std::int64_t peer_id, std::string_view text, attachment::attachments_t list) const;
    void send(std::int64_t peer_id, std::string_view text, std::map<std::string, std::string> raw_parameters) const;
    void send(std::int64_t peer_id, std::string_view text, std::string_view layout) const;
    void remove_chat_user(std::int64_t chat_id, std::int64_t user_id) const;
    void edit_chat(std::int64_t chat_id, std::string_view new_title) const;
    void create_chat(std::string_view title, std::int64_t group_id, std::vector<std::size_t> user_ids);
    void add_chat_user(std::int64_t chat_id, std::int64_t user_id);
    void delete_chat_photo(std::int64_t chat_id, std::int64_t group_id) const;
    void pin(std::int64_t peer_id, std::int64_t message_id, std::int64_t conversation_message_id) const;
    void set_chat_photo(std::string_view filename, std::string_view raw_server) const;
    conversation_member_list get_conversation_members(std::int64_t peer_id) const;

protected:
    bool m_disable_mentions_flag;
    std::shared_ptr<simdjson::dom::parser> m_parser;
    document::common m_document;
    mutable method::group_constructor_proxy m_group_constructor;
    mutable method::user_constructor_proxy m_user_constructor;
};
}// namespace method
}// namespace vk

#endif// VK_MESSAGES_H
