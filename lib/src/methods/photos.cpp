#include <simdjson.h>

#include "lib/include/utility/exception.hpp"
#include "lib/include/methods/photos.hpp"


vk::attachment::attachments_t vk::photos::search(std::string_view query, std::int64_t count)
{
    return common_search("photos.search", query, count);
}

std::string vk::photos::get_messages_upload_server(std::int64_t peer_id)
{
    return call("photos.getMessagesUploadServer", group_params({
        {"peer_id", std::to_string(peer_id)}
    }));
}

std::string vk::photos::get_chat_upload_server(std::int64_t chat_id)
{
    return call("photos.getChatUploadServer", group_params({
        {"crop_x",     "512"},
        {"crop_y",     "512"},
        {"chat_id",    std::to_string(chat_id - 2000000000)}
    }));
}

std::shared_ptr<vk::attachment::photo_attachment> vk::photos::save_messages_photo(std::string_view filename, std::string_view raw_server)
{
    simdjson::dom::object upload_response = common_upload(parser, network_client, filename, raw_server, "file");

    if (static_cast<std::string_view>(upload_response["photo"]) == "[]" ||
        static_cast<std::string_view>(upload_response["photo"]) == "")
    {
        VK_THROW(exception::upload_error, -1, "Can't upload file. Maybe is not an image?");
    }

    std::string raw_vk_response =
    call("photos.saveMessagesPhoto", group_params({
        {"photo",   static_cast<std::string>(upload_response["photo"])},
        {"hash",    static_cast<std::string>(upload_response["hash"])},
        {"server",  std::to_string(static_cast<std::int64_t>(upload_response["server"]))}
    }));

    simdjson::dom::object uploaded_attachment = parser.parse(raw_vk_response)["response"].at(0);

    std::int64_t owner_id = static_cast<std::int64_t>(uploaded_attachment["id"]);
    std::int64_t id       = static_cast<std::int64_t>(uploaded_attachment["owner_id"]);

    return std::make_shared<vk::attachment::photo_attachment>(id, owner_id);
}
