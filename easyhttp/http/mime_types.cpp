#include "mime_types.h"

namespace easyhttp 
{

std::unordered_map<std::string, std::string> mime_types::mime_type_map_ =
{
    { "gif", "image/gif" },
    { "htm", "text/html" },
    { "html", "text/html" },
    { "jpg", "image/jpeg" },
    { "png", "image/png" }
};

std::string mime_types::get_mime_type(const std::string& extension)
{
    auto iter = mime_type_map_.find(extension);
    if (iter != mime_type_map_.end())
    {
        return iter->second;
    }

    return "text/plain";
}

}
