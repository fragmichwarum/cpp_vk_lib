#ifndef STRING_UTIL_SPLIT_IMPL_H
#define STRING_UTIL_SPLIT_IMPL_H

#include <string_view>
#include <vector>

namespace vk {
namespace string_utils {
std::vector<std::string_view> split(std::string_view text, char delimiter);
}// namespace string_utils
}// namespace vk

namespace vk {
namespace string_utils {

struct split_impl
{
public:
    split_impl() = delete;

private:
    static std::vector<std::string_view> create(std::string_view data, char delimiter)
    {
        std::vector<std::string_view> splitted;
        splitted.reserve(data.length() / 4);
        std::size_t pos = 0;
        while (pos != std::string_view::npos)
        {
            pos = data.find_first_not_of(delimiter);
            if (pos == std::string_view::npos)
            {
                return splitted;
            }
            data = data.substr(pos);
            pos = data.find(delimiter);
            splitted.emplace_back(data.substr(0, pos));
            data = data.substr(pos == std::string_view::npos ? 0 : pos);
        }
        splitted.shrink_to_fit();
        return splitted;
    }

    friend std::vector<std::string_view> string_utils::split(std::string_view text, char delimiter);
};
}// namespace string_utils
}// namespace vk

#endif// STRING_UTIL_SPLIT_IMPL_H
