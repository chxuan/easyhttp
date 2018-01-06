#include "http_parser.h"

http_parser::http_parser()
{
    reset();
}

void http_parser::reset()
{
    state_ = parse_state::method_start;
}

parse_result http_parser::parse_each_char(request& req, char ch)
{
    switch (state_)
    {
    case parse_state::method_start: return deal_method_start(req, ch);
    case parse_state::method: return deal_method(req, ch);
    case parse_state::uri: return deal_uri(req, ch);
    case parse_state::http_version_h: return deal_http_version_h(ch);
    case parse_state::http_version_t_1: return deal_http_version_t_1(ch);
    case parse_state::http_version_t_2: return deal_http_version_t_2(ch);
    case parse_state::http_version_p: return deal_http_version_p(ch);
    case parse_state::http_version_slash: return deal_http_version_slash(req, ch);
    case parse_state::http_version_major_start: return deal_http_version_major_start(req, ch);
    case parse_state::http_version_major: return deal_http_version_major(req, ch);
    case parse_state::http_version_minor_start: return deal_http_version_minor_start(req, ch);
    case parse_state::http_version_minor: return deal_http_version_minor(req, ch);
    case parse_state::expecting_newline_1: return deal_expecting_newline_1(ch);
    case parse_state::header_line_start: return deal_header_line_start(req, ch);
    case parse_state::header_lws: return deal_header_lws(req, ch);
    case parse_state::header_name: return deal_header_name(req, ch);
    case parse_state::space_before_header_value: return deal_space_before_header_value(ch);
    case parse_state::header_value: return deal_header_value(req, ch);
    case parse_state::expecting_newline_2: return deal_expecting_newline_2(ch);
    case parse_state::expecting_newline_3: return deal_expecting_newline_3(ch);
    default: return parse_result::error;
    }
}

parse_result http_parser::deal_method_start(request& req, char ch)
{
    if (!is_char(ch) || is_ctl(ch) || is_tspecial(ch))
    {
        return parse_result::error;
    }
    else
    {
        state_ = parse_state::method;
        req.method.push_back(ch);
        return parse_result::indeterminate;
    }
}

parse_result http_parser::deal_method(request& req, char ch)
{
    if (ch == ' ')
    {
        state_ = parse_state::uri;
        return parse_result::indeterminate;
    }
    else if (!is_char(ch) || is_ctl(ch) || is_tspecial(ch))
    {
        return parse_result::error;
    }
    else
    {
        req.method.push_back(ch);
        return parse_result::indeterminate;
    }
}

parse_result http_parser::deal_uri(request& req, char ch)
{
    if (ch == ' ')
    {
        state_ = parse_state::http_version_h;
        return parse_result::indeterminate;
    }
    else if (is_ctl(ch))
    {
        return parse_result::error;
    }
    else
    {
        req.uri.push_back(ch);
        return parse_result::indeterminate;
    }
}

parse_result http_parser::deal_http_version_h(char ch)
{
    if (ch == 'H')
    {
        state_ = parse_state::http_version_t_1;
        return parse_result::indeterminate;
    }

    return parse_result::error;
}

parse_result http_parser::deal_http_version_t_1(char ch)
{
    if (ch == 'T')
    {
        state_ = parse_state::http_version_t_2;
        return parse_result::indeterminate;
    }

    return parse_result::error;
}

parse_result http_parser::deal_http_version_t_2(char ch)
{
    if (ch == 'T')
    {
        state_ = parse_state::http_version_p;
        return parse_result::indeterminate;
    }

    return parse_result::error;
}

parse_result http_parser::deal_http_version_p(char ch)
{
    if (ch == 'P')
    {
        state_ = parse_state::http_version_slash;
        return parse_result::indeterminate;
    }

    return parse_result::error;
}

parse_result http_parser::deal_http_version_slash(request& req, char ch)
{
    if (ch == '/')
    {
        req.http_version_major = 0;
        req.http_version_minor = 0;
        state_ = parse_state::http_version_major_start;
        return parse_result::indeterminate;
    }

    return parse_result::error;
}

parse_result http_parser::deal_http_version_major_start(request& req, char ch)
{
    if (is_digit(ch))
    {
        req.http_version_major = req.http_version_major * 10 + ch - '0';
        state_ = parse_state::http_version_major;
        return parse_result::indeterminate;
    }

    return parse_result::error;
}

parse_result http_parser::deal_http_version_major(request& req, char ch)
{
    if (ch == '.')
    {
        state_ = parse_state::http_version_minor_start;
        return parse_result::indeterminate;
    }
    else if (is_digit(ch))
    {
        req.http_version_major = req.http_version_major * 10 + ch - '0';
        return parse_result::indeterminate;
    }

    return parse_result::error;
}

parse_result http_parser::deal_http_version_minor_start(request& req, char ch)
{
    if (is_digit(ch))
    {
        req.http_version_minor = req.http_version_minor * 10 + ch - '0';
        state_ = parse_state::http_version_minor;
        return parse_result::indeterminate;
    }

    return parse_result::error;
}

parse_result http_parser::deal_http_version_minor(request& req, char ch)
{
    if (is_line_end(ch))
    {
        state_ = parse_state::expecting_newline_1;
        return parse_result::indeterminate;
    }
    else if (is_digit(ch))
    {
        req.http_version_minor = req.http_version_minor * 10 + ch - '0';
        return parse_result::indeterminate;
    }

    return parse_result::error;
}

parse_result http_parser::deal_expecting_newline_1(char ch)
{
    if (is_line_begin(ch))
    {
        state_ = parse_state::header_line_start;
        return parse_result::indeterminate;
    }

    return parse_result::error;
}

parse_result http_parser::deal_header_line_start(request& req, char ch)
{
    if (is_line_end(ch))
    {
        state_ = parse_state::expecting_newline_3;
        return parse_result::indeterminate;
    }
    else if (!req.headers.empty() && (ch == ' ' || ch == '\t'))
    {
        state_ = parse_state::header_lws;
        return parse_result::indeterminate;
    }
    else if (!is_char(ch) || is_ctl(ch) || is_tspecial(ch))
    {
        return parse_result::error;
    }
    else
    {
        req.headers.emplace_back(header());
        req.headers.back().name.push_back(ch);
        state_ = parse_state::header_name;
        return parse_result::indeterminate;
    }
}

parse_result http_parser::deal_header_lws(request& req, char ch)
{
    if (is_line_end(ch))
    {
        state_ = parse_state::expecting_newline_2;
        return parse_result::indeterminate;
    }
    else if (ch == ' ' || ch == '\t')
    {
        return parse_result::indeterminate;
    }
    else if (is_ctl(ch))
    {
        return parse_result::error;
    }
    else
    {
        state_ = parse_state::header_value;
        req.headers.back().value.push_back(ch);
        return parse_result::indeterminate;
    }
}

parse_result http_parser::deal_header_name(request& req, char ch)
{
    if (ch == ':')
    {
        state_ = parse_state::space_before_header_value;
        return parse_result::indeterminate;
    }
    else if (!is_char(ch) || is_ctl(ch) || is_tspecial(ch))
    {
        return parse_result::error;
    }
    else
    {
        req.headers.back().name.push_back(ch);
        return parse_result::indeterminate;
    }
}

parse_result http_parser::deal_space_before_header_value(char ch)
{
    if (ch == ' ')
    {
        state_ = parse_state::header_value;
        return parse_result::indeterminate;
    }

    return parse_result::error;
}

parse_result http_parser::deal_header_value(request& req, char ch)
{
    if (is_line_end(ch))
    {
        state_ = parse_state::expecting_newline_2;
        return parse_result::indeterminate;
    }
    else if (is_ctl(ch))
    {
        return parse_result::error;
    }
    else
    {
        req.headers.back().value.push_back(ch);
        return parse_result::indeterminate;
    }
}

parse_result http_parser::deal_expecting_newline_2(char ch)
{
    if (is_line_begin(ch))
    {
        state_ = parse_state::header_line_start;
        return parse_result::indeterminate;
    }

    return parse_result::error;
}

parse_result http_parser::deal_expecting_newline_3(char ch)
{
    return (ch == '\n') ? parse_result::finished : parse_result::error;
}

bool http_parser::is_char(int ch)
{
    return ch >= 0 && ch <= 127;
}

bool http_parser::is_ctl(int ch)
{
    return (ch >= 0 && ch <= 31) || (ch == 127);
}

bool http_parser::is_tspecial(char ch)
{
    switch (ch)
    {
    case '(': case ')': case '<': case '>': case '@':
    case ',': case ';': case ':': case '\\': case '"':
    case '/': case '[': case ']': case '?': case '=':
    case '{': case '}': case ' ': case '\t': 
        return true;
    default: 
        return false;
    }
}

bool http_parser::is_digit(char ch)
{
    return ch >= '0' && ch <= '9';
}

bool http_parser::is_line_begin(char ch)
{
    return ch == '\n';
}

bool http_parser::is_line_end(char ch)
{
    return ch == '\r';
}
