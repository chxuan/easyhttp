/**
 * @file http_server_test.h
 * @brief http服务测试程序
 * @author chxuan, 787280310@qq.com
 * @version 1.0.0
 * @date 2018-01-06
 */
#pragma once

#include "easyhttp/easyhttp.h"

class http_server_test
{
public:
    http_server_test();
    ~http_server_test();

    void run();
    void stop();

private:
    std::shared_ptr<http_server> server_;
};
