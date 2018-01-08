/**
 * @file http_parser.h
 * @brief http协议解析器
 * @author chxuan, 787280310@qq.com
 * @version 1.0.0
 * @date 2018-01-06
 */
#pragma once

#include <memory>
#include <vector>
#include "header.h"
#include "status_types.h"

namespace easyhttp 
{

class request;

enum class parse_result : unsigned char
{
    indeterminate,
    finished,
    error
};

enum class parse_state : unsigned char
{
    method_start,
    method,
    uri,
    param_name_start,
    param_name,
    param_value,
    http_version_h,
    http_version_t_1,
    http_version_t_2,
    http_version_p,
    http_version_slash,
    http_main_version_start,
    http_main_version,
    http_sub_version_start,
    http_sub_version,
    expecting_newline_1,
    header_line_start,
    header_lws,
    header_name,
    space_before_header_value,
    header_value,
    expecting_newline_2,
    expecting_newline_3,
    body
};

class http_parser
{
public:
    parse_result parse(std::shared_ptr<request>& req, char* begin, char* end);
    std::shared_ptr<std::string> pack(status_type type, const std::string& body = "");

private:
    parse_result parse_each_char(std::shared_ptr<request>& req, char ch);

    parse_result deal_method_start(std::shared_ptr<request>& req, char ch);
    parse_result deal_method(std::shared_ptr<request>& req, char ch);
    parse_result deal_uri(std::shared_ptr<request>& req, char ch);
    parse_result deal_param_name_start(std::shared_ptr<request>& req, char ch);
    parse_result deal_param_name(std::shared_ptr<request>& req, char ch);
    parse_result deal_param_value(std::shared_ptr<request>& req, char ch);
    parse_result deal_http_version_h(char ch);
    parse_result deal_http_version_t_1(char ch);
    parse_result deal_http_version_t_2(char ch);
    parse_result deal_http_version_p(char ch);
    parse_result deal_http_version_slash(std::shared_ptr<request>& req, char ch);
    parse_result deal_http_main_version_start(std::shared_ptr<request>& req, char ch);
    parse_result deal_http_main_version(std::shared_ptr<request>& req, char ch);
    parse_result deal_http_sub_version_start(std::shared_ptr<request>& req, char ch);
    parse_result deal_http_sub_version(std::shared_ptr<request>& req, char ch);
    parse_result deal_expecting_newline_1(char ch);
    parse_result deal_header_line_start(std::shared_ptr<request>& req, char ch);
    parse_result deal_header_lws(std::shared_ptr<request>& req, char ch);
    parse_result deal_header_name(std::shared_ptr<request>& req, char ch);
    parse_result deal_space_before_header_value(char ch);
    parse_result deal_header_value(std::shared_ptr<request>& req, char ch);
    parse_result deal_expecting_newline_2(char ch);
    parse_result deal_expecting_newline_3(std::shared_ptr<request>& req, char ch);
    parse_result deal_body(std::shared_ptr<request>& req, char ch);

    std::vector<header> pack_header(int body_len);
    std::shared_ptr<std::string> make_network_data(status_type type, 
                                                   const std::vector<header>& headers, 
                                                   const std::string& body);

    static bool is_char(int ch);
    static bool is_ctl(int ch);
    static bool is_tspecial(char ch);
    static bool is_digit(char ch);
    static bool is_line_begin(char ch);
    static bool is_line_end(char ch);

private:
    parse_state state_ = parse_state::method_start;
    int body_len_ = 0;
    int pos_ = 0;
};

}
