#include "status_types.h"

std::map<status_type, std::string> status_types::status_type_map_ = 
{
    { status_type::ok, "HTTP/1.0 200 OK" },
    { status_type::created, "HTTP/1.0 201 Created" },
    { status_type::accepted, "HTTP/1.0 202 Accepted" },
    { status_type::no_content, "HTTP/1.0 204 No Content" },
    { status_type::multiple_choices, "HTTP/1.0 300 Multiple Choices" },
    { status_type::moved_permanently, "HTTP/1.0 301 Moved Permanently" },
    { status_type::moved_temporarily, "HTTP/1.0 302 Moved Temporarily" },
    { status_type::not_modified, "HTTP/1.0 304 Not Modified" },
    { status_type::bad_request, "HTTP/1.0 400 Bad Request" },
    { status_type::unauthorized, "HTTP/1.0 401 Unauthorized" },
    { status_type::forbidden, "HTTP/1.0 403 Forbidden" },
    { status_type::not_found, "HTTP/1.0 404 Not Found" },
    { status_type::internal_server_error, "HTTP/1.0 500 Internal Server Error" },
    { status_type::not_implemented, "HTTP/1.0 501 Not Implemented" },
    { status_type::bad_gateway, "HTTP/1.0 502 Bad Gateway" },
    { status_type::service_unavailable, "HTTP/1.0 503 Service Unavailable" }
};

std::string status_types::to_string(status_type type)
{
    return status_type_map_[type];
}
