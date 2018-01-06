#include "tcp_server.h"
#include "tcp_session.h"
#include "io_service_pool.h"
#include "easyhttp/utility/logger.h"
#include "easyhttp/utility/utiltiy.h"

tcp_server::tcp_server(const std::string& host, int ios_threads)
    : host_(host),
    pool_(std::make_shared<io_service_pool>(ios_threads)), 
    acceptor_(pool_->get_io_service())
{

}

tcp_server::~tcp_server()
{
    stop();
}

bool tcp_server::run()
{
    return start_listen();
}

void tcp_server::stop()
{
    pool_->stop();
}

bool tcp_server::start_listen()
{
    pool_->run();

    std::string ip;
    unsigned short port = 0;
    if (!utiltiy::get_ip_and_port(host_, ip, port))
    {
        log_error << "Parse address failed, host: " << host_;
        return false;
    }

    if (listen(ip, port))
    {
        accept();
        return true;
    }

    return false;
}

bool tcp_server::listen(const std::string& ip, unsigned short port)
{
    try
    {
        boost::asio::ip::tcp::endpoint ep(boost::asio::ip::address_v4::from_string(ip), port);
        acceptor_.open(ep.protocol());
        acceptor_.set_option(boost::asio::ip::tcp::acceptor::reuse_address(true));
        acceptor_.bind(ep);
        acceptor_.listen();
    }
    catch(std::exception& e)
    {
        log_error << e.what();
        return false;
    }

    return true;
}

void tcp_server::accept()
{
    auto session = std::make_shared<tcp_session>(pool_->get_io_service());
    acceptor_.async_accept(session->get_socket(), [this, session](boost::system::error_code ec)
    {
        if (!ec)
        {
            session->run();
        }
        accept();
    });
}
