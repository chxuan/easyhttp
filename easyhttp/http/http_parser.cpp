#include "http_parser.h"
#include "request.h"

static const std::string crlf = "\r\n";

parse_result http_parser::parse(std::shared_ptr<request>& req, char* begin, char* end)
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

std::shared_ptr<std::string> http_parser::pack(status_type type, const std::string& body)
{
    return make_network_data(type, pack_header(body.size()), body);
}

parse_result http_parser::parse_each_char(std::shared_ptr<request>& req, char ch)
{
    switch (state_)
    {
    case parse_state::method_start: return deal_method_start(req, ch);
    case parse_state::method: return deal_method(req, ch);
    case parse_state::uri: return deal_uri(req, ch);
    case parse_state::param_name_start: return deal_param_name_start(req, ch);
    case parse_state::param_name: return deal_param_name(req, ch);
    case parse_state::param_value: return deal_param_value(req, ch);
    case parse_state::http_version_h: return deal_http_version_h(ch);
    case parse_state::http_version_t_1: return deal_http_version_t_1(ch);
    case parse_state::http_version_t_2: return deal_http_version_t_2(ch);
    case parse_state::http_version_p: return deal_http_version_p(ch);
    case parse_state::http_version_slash: return deal_http_version_slash(req, ch);
    case parse_state::http_main_version_start: return deal_http_main_version_start(req, ch);
    case parse_state::http_main_version: return deal_http_main_version(req, ch);
    case parse_state::http_sub_version_start: return deal_http_sub_version_start(req, ch);
    case parse_state::http_sub_version: return deal_http_sub_version(req, ch);
    case parse_state::expecting_newline_1: return deal_expecting_newline_1(ch);
    case parse_state::header_line_start: return deal_header_line_start(req, ch);
    case parse_state::header_lws: return deal_header_lws(req, ch);
    case parse_state::header_name: return deal_header_name(req, ch);
    case parse_state::space_before_header_value: return deal_space_before_header_value(ch);
    case parse_state::header_value: return deal_header_value(req, ch);
    case parse_state::expecting_newline_2: return deal_expecting_newline_2(ch);
    case parse_state::expecting_newline_3: return deal_expecting_newline_3(req, ch);
    case parse_state::body: return deal_body(req, ch);
    default: return parse_result::error;
    }
}

parse_result http_parser::deal_method_start(std::shared_ptr<request>& req, char ch)
{
    if (!is_char(ch) || is_ctl(ch) || is_tspecial(ch))
    {
        return parse_result::error;
    }
    else
    {
        state_ = parse_state::method;
        req->method.push_back(ch);
        return parse_result::indeterminate;
    }
}

parse_result http_parser::deal_method(std::shared_ptr<request>& req, char ch)
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
        req->method.push_back(ch);
        return parse_result::indeterminate;
    }
}

parse_result http_parser::deal_uri(std::shared_ptr<request>& req, char ch)
{
    if (ch == ' ')
    {
        state_ = parse_state::http_version_h;
        return parse_result::indeterminate;
    }
    else if (ch == '?')
    {
        state_ = parse_state::param_name_start;
        return parse_result::indeterminate;
    }
    else if (is_ctl(ch))
    {
        return parse_result::error;
    }
    else
    {
        req->uri.push_back(ch);
        return parse_result::indeterminate;
    }
}

parse_result http_parser::deal_param_name_start(std::shared_ptr<request>& req, char ch)
{
    if (!is_char(ch) || is_ctl(ch) || is_tspecial(ch))
    {
        return parse_result::error;
    }
    else
    {
        req->params.emplace_back(param());
        req->params.back().name.push_back(ch);
        state_ = parse_state::param_name;
        return parse_result::indeterminate;
    }
}

parse_result http_parser::deal_param_name(std::shared_ptr<request>& req, char ch)
{
    if (ch == '=')
    {
        state_ = parse_state::param_value;
        return parse_result::indeterminate;
    }
    else if (!is_char(ch) || is_ctl(ch) || is_tspecial(ch))
    {
        return parse_result::error;
    }
    else
    {
        req->params.back().name.push_back(ch);
        return parse_result::indeterminate;
    }
}

parse_result http_parser::deal_param_value(std::shared_ptr<request>& req, char ch)
{
    if (ch == '&')
    {
        state_ = parse_state::param_name_start;
        return parse_result::indeterminate;
    }
    else if (ch == ' ')
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
        req->params.back().value.push_back(ch);
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

parse_result http_parser::deal_http_version_slash(std::shared_ptr<request>& req, char ch)
{
    if (ch == '/')
    {
        req->http_main_version = 0;
        req->http_sub_version = 0;
        state_ = parse_state::http_main_version_start;
        return parse_result::indeterminate;
    }

    return parse_result::error;
}

parse_result http_parser::deal_http_main_version_start(std::shared_ptr<request>& req, char ch)
{
    if (is_digit(ch))
    {
        req->http_main_version = req->http_main_version * 10 + ch - '0';
        state_ = parse_state::http_main_version;
        return parse_result::indeterminate;
    }

    return parse_result::error;
}

parse_result http_parser::deal_http_main_version(std::shared_ptr<request>& req, char ch)
{
    if (ch == '.')
    {
        state_ = parse_state::http_sub_version_start;
        return parse_result::indeterminate;
    }
    else if (is_digit(ch))
    {
        req->http_main_version = req->http_main_version * 10 + ch - '0';
        return parse_result::indeterminate;
    }

    return parse_result::error;
}

parse_result http_parser::deal_http_sub_version_start(std::shared_ptr<request>& req, char ch)
{
    if (is_digit(ch))
    {
        req->http_sub_version = req->http_sub_version * 10 + ch - '0';
        state_ = parse_state::http_sub_version;
        return parse_result::indeterminate;
    }

    return parse_result::error;
}

parse_result http_parser::deal_http_sub_version(std::shared_ptr<request>& req, char ch)
{
    if (is_line_end(ch))
    {
        state_ = parse_state::expecting_newline_1;
        return parse_result::indeterminate;
    }
    else if (is_digit(ch))
    {
        req->http_sub_version = req->http_sub_version * 10 + ch - '0';
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

parse_result http_parser::deal_header_line_start(std::shared_ptr<request>& req, char ch)
{
    if (is_line_end(ch))
    {
        state_ = parse_state::expecting_newline_3;
        return parse_result::indeterminate;
    }
    else if (!req->headers.empty() && (ch == ' ' || ch == '\t'))
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
        req->headers.emplace_back(header());
        req->headers.back().name.push_back(ch);
        state_ = parse_state::header_name;
        return parse_result::indeterminate;
    }
}

parse_result http_parser::deal_header_lws(std::shared_ptr<request>& req, char ch)
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
        req->headers.back().value.push_back(ch);
        return parse_result::indeterminate;
    }
}

parse_result http_parser::deal_header_name(std::shared_ptr<request>& req, char ch)
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
        req->headers.back().name.push_back(ch);
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

parse_result http_parser::deal_header_value(std::shared_ptr<request>& req, char ch)
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
        req->headers.back().value.push_back(ch);
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

parse_result http_parser::deal_expecting_newline_3(std::shared_ptr<request>& req, char ch)
{
    if (is_line_begin(ch))
    {
        if (req->method == "POST")
        {
            body_len_ = std::atoi(req->get_header_value("Content-Length").c_str());
            if (body_len_ != 0)
            {
                req->body.resize(body_len_);
                state_ = parse_state::body;
                return parse_result::indeterminate;
            }
        }

        return parse_result::finished;
    }
    else
    {
        return parse_result::error;
    }
}

parse_result http_parser::deal_body(std::shared_ptr<request>& req, char ch)
{
    req->body[pos_++] = ch;
    return pos_ != body_len_ ? parse_result::indeterminate : parse_result::finished;
}

std::vector<header> http_parser::pack_header(int body_len)
{
    std::vector<header> headers;
    headers.emplace_back(header{ "Content-Length", std::to_string(body_len) });
    headers.emplace_back(header{ "Content-Type", "text/plain" });

    return headers;
}

std::shared_ptr<std::string> http_parser::make_network_data(status_type type, 
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
