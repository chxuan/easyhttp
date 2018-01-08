/**
 * @file router.h
 * @brief http服务消息路由器
 * @author chxuan, 787280310@qq.com
 * @version 1.0.0
 * @date 2018-01-07
 */
#pragma once

#include <unordered_map>
#include "easyhttp/utility/thread_pool.h"

class request;
class response;

using request_handler = std::function<void(const std::shared_ptr<request>&, const std::shared_ptr<response>&)>;

class router
{
public:
    ~router();

    void run(int work_threads);
    std::size_t route_table_size();
    void route(const std::string& uri, const request_handler& func);
    void stop();
    void do_route(const std::shared_ptr<request>& req, const std::shared_ptr<response>& res);

private:
    void route_thread(const std::shared_ptr<request>& req, const std::shared_ptr<response>& res);

private:
    thread_pool threadpool_;
    std::unordered_map<std::string, request_handler> route_table_;
};
