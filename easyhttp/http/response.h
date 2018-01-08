/**
 * @file response.h
 * @brief http应答
 * @author chxuan, 787280310@qq.com
 * @version 1.0.0
 * @date 2018-01-07
 */
#pragma once

#include "easyhttp/net/tcp_session.h"
#include "easyhttp/http/http_parser.h"
#include "easyhttp/http/status_types.h"

namespace easyhttp 
{

struct response
{
    response(const std::shared_ptr<tcp_session>& sess)
        : session(sess) {}

    void set_response(status_type type)
    {
        session->async_write(session->get_parser()->pack(type));
    }

    void set_response(const std::string& body)
    {
        session->async_write(session->get_parser()->pack(status_type::ok, body));
    }

    std::shared_ptr<tcp_session> session;
};

}
