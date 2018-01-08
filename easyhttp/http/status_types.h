/**
 * @file status_types.h
 * @brief 状态类型映射关系
 * @author chxuan, 787280310@qq.com
 * @version 1.0.0
 * @date 2018-01-07
 */
#pragma once

#include <string>
#include <map>

namespace easyhttp 
{

enum class status_type
{
    ok = 200,
    created = 201,
    accepted = 202,
    no_content = 204,
    multiple_choices = 300,
    moved_permanently = 301,
    moved_temporarily = 302,
    not_modified = 304,
    bad_request = 400,
    unauthorized = 401,
    forbidden = 403,
    not_found = 404,
    internal_server_error = 500,
    not_implemented = 501,
    bad_gateway = 502,
    service_unavailable = 503
};

class status_types
{
public:
    static std::string to_string(status_type type);

private:
    static std::map<status_type, std::string> status_type_map_;
};

}
