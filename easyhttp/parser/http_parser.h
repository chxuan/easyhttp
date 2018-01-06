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

struct request
{
    std::string method;
    std::string uri;
    int http_version_major;
    int http_version_minor;
    std::vector<header> headers;
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
    http_version_h,
    http_version_t_1,
    http_version_t_2,
    http_version_p,
    http_version_slash,
    http_version_major_start,
    http_version_major,
    http_version_minor_start,
    http_version_minor,
    expecting_newline_1,
    header_line_start,
    header_lws,
    header_name,
    space_before_header_value,
    header_value,
    expecting_newline_2,
    expecting_newline_3
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
    parse_result deal_http_version_h(char ch);
    parse_result deal_http_version_t_1(char ch);
    parse_result deal_http_version_t_2(char ch);
    parse_result deal_http_version_p(char ch);
    parse_result deal_http_version_slash(request& req, char ch);
    parse_result deal_http_version_major_start(request& req, char ch);
    parse_result deal_http_version_major(request& req, char ch);
    parse_result deal_http_version_minor_start(request& req, char ch);
    parse_result deal_http_version_minor(request& req, char ch);
    parse_result deal_expecting_newline_1(char ch);
    parse_result deal_header_line_start(request& req, char ch);
    parse_result deal_header_lws(request& req, char ch);
    parse_result deal_header_name(request& req, char ch);
    parse_result deal_space_before_header_value(char ch);
    parse_result deal_header_value(request& req, char ch);
    parse_result deal_expecting_newline_2(char ch);
    parse_result deal_expecting_newline_3(char ch);

    static bool is_char(int ch);
    static bool is_ctl(int ch);
    static bool is_tspecial(char ch);
    static bool is_digit(char ch);
    static bool is_line_begin(char ch);
    static bool is_line_end(char ch);

private:
    parse_state state_;
};

