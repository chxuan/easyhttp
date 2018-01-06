/**
 * @file tcp_server.h
 * @brief tcp服务端
 * @author chxuan, 787280310@qq.com
 * @version 1.0.0
 * @date 2017-11-16
 */
#pragma once

#include <boost/asio.hpp>
#include <google/protobuf/message.h>

class io_service_pool;
class tcp_session;

class tcp_server
{
public:
    tcp_server(const std::string& host, int ios_threads);
    virtual ~tcp_server();

    virtual bool run();
    virtual void stop();

private:
    bool start_listen();
    bool listen(const std::string& ip, unsigned short port);
    void accept();

private:
    std::string host_;
    std::shared_ptr<io_service_pool> pool_;
    boost::asio::ip::tcp::acceptor acceptor_;
};
