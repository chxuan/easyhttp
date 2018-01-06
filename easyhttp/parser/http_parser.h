/**
 * @file http_parser.h
 * @brief http协议解析器
 * @author chxuan, 787280310@qq.com
 * @version 1.0.0
 * @date 2018-01-06
 */
#pragma once

#include <string>
#include <vector>

struct header
{
    std::string name;
    std::string value;
};

struct param
{
    std::string name;
    std::string value;
};

struct request
{
    std::string get_param(const std::string& name)
    {
        for (auto& param : params)
        {
            if (param.name == name)
            {
                return param.value;
            }
        }

        return "";
    }
    std::string get_header(const std::string& name)
    {
        for (auto& header : headers)
        {
            if (header.name == name)
            {
                return header.value;
            }
        }

        return "";
    }
    std::string method;
    std::string uri;
    std::vector<param> params;
    int http_main_version;
    int http_sub_version;
    std::vector<header> headers;
    std::string body;
};

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
    http_parser();

    void reset();

    template <typename InputIterator>
    parse_result parse(request& req, InputIterator begin, InputIterator end)
    {
        while (begin != end)
        {
            parse_result ret = parse_each_char(req, *begin++);
            if (ret != parse_result::indeterminate)
            {
                return ret;
            }
        }

        return parse_result::indeterminate;
    }

private:
    parse_result parse_each_char(request& req, char ch);

    parse_result deal_method_start(request& req, char ch);
    parse_result deal_method(request& req, char ch);
    parse_result deal_uri(request& req, char ch);
    parse_result deal_param_name_start(request& req, char ch);
    parse_result deal_param_name(request& req, char ch);
    parse_result deal_param_value(request& req, char ch);
    parse_result deal_http_version_h(char ch);
    parse_result deal_http_version_t_1(char ch);
    parse_result deal_http_version_t_2(char ch);
    parse_result deal_http_version_p(char ch);
    parse_result deal_http_version_slash(request& req, char ch);
    parse_result deal_http_main_version_start(request& req, char ch);
    parse_result deal_http_main_version(request& req, char ch);
    parse_result deal_http_sub_version_start(request& req, char ch);
    parse_result deal_http_sub_version(request& req, char ch);
    parse_result deal_expecting_newline_1(char ch);
    parse_result deal_header_line_start(request& req, char ch);
    parse_result deal_header_lws(request& req, char ch);
    parse_result deal_header_name(request& req, char ch);
    parse_result deal_space_before_header_value(char ch);
    parse_result deal_header_value(request& req, char ch);
    parse_result deal_expecting_newline_2(char ch);
    parse_result deal_expecting_newline_3(request& req, char ch);
    parse_result deal_body(request& req, char ch);

    static bool is_char(int ch);
    static bool is_ctl(int ch);
    static bool is_tspecial(char ch);
    static bool is_digit(char ch);
    static bool is_line_begin(char ch);
    static bool is_line_end(char ch);

private:
    parse_state state_;
    int body_len_ = 0;
    int pos_ = 0;
};

