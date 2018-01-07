/**
 * @file response.h
 * @brief http应答
 * @author chxuan, 787280310@qq.com
 * @version 1.0.0
 * @date 2018-01-07
 */
#pragma once

#include "easyhttp/net/tcp_session.h"

struct response
{
    response(const std::shared_ptr<tcp_session>& sess)
        : session(sess) {}

    std::shared_ptr<tcp_session> session;
};

