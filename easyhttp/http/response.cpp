#include "response.h"

static const std::string crlf = "\r\n";

std::shared_ptr<std::string> response::pack(status_type type, const std::string& body)
{
    return make_network_data(type, pack_header(body.size()), body);
}

std::vector<header> response::pack_header(int body_len)
{
    std::vector<header> headers;
    headers.emplace_back(header{ "Content-Length", std::to_string(body_len) });
    headers.emplace_back(header{ "Content-Type", "text/plain" });

    return headers;
}

std::shared_ptr<std::string> response::make_network_data(status_type type, 
                                                         const std::vector<header>& headers, 
                                                         const std::string& body)
{
    auto buffer = std::make_shared<std::string>();

    buffer->append(status_types::to_string(type) + crlf);
    for (auto& header : headers)
    {
        buffer->append(header.name + ": " + header.value + crlf);
    }
    buffer->append(crlf);
    buffer->append(body);

    return buffer;
}
