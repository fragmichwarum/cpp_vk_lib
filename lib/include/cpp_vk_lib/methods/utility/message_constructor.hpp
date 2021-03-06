#ifndef VK_MESSAGE_CONSTRUCTOR_H
#define VK_MESSAGE_CONSTRUCTOR_H

#include "attachment/attachment.hpp"
#include "methods/utility/constructor.hpp"

#include <iostream>
#include <map>
#include <numeric>
#include <string>

namespace vk {
namespace method {
/*!
 * @brief Adapter to work with message.send method only.
 */
class message_constructor
{
public:
    static inline bool disable_mentions = true;
    static inline bool enable_mentions = false;

    message_constructor(bool disable_mentions_flag)
    {
        m_constructor.method("messages.send");

        param("random_id", "0");

        if (disable_mentions_flag)
        {
            param("disable_mentions", "1");
        }
        else {
            param("disable_mentions", "0");
        }
    }

    message_constructor& param(std::string_view lhs, std::string_view rhs)
    {
        m_constructor.param(lhs, rhs);
        return *this;
    }
    /*!
     * Move or copy is your choice.
     */
    message_constructor& append_map(std::map<std::string, std::string> additional_params)
    {
        m_constructor.append_map(std::move(additional_params));
        return *this;
    }

    message_constructor& attachments(attachment::attachments_t attachments)
    {
        param("attachment", append_attachments_impl(std::move(attachments)).data());
        return *this;
    }

    std::string execute()
    {
        return m_constructor.execute();
    }

private:
    std::string append_attachments_impl(attachment::attachments_t attachments) const
    {
        std::string result;
        result.reserve(attachments.size() * 20);

        for (auto& attachment : attachments)
        {
            result += attachment->value();
            result += ',';
        }

        return result;
    }

    group_constructor_proxy m_constructor;
};

}// namespace method
}// namespace vk

#endif// VK_MESSAGE_CONSTRUCTOR_H
