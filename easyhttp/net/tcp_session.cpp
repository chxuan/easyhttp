#include "tcp_session.h"
#include "easyhttp/http/request.h"
#include "easyhttp/http/response.h"
#include "easyhttp/http/http_parser.h"
#include "easyhttp/utility/logger.h"

tcp_session::tcp_session(boost::asio::io_service& ios, const request_handler& func)
    : ios_(ios), 
    socket_(ios),
    func_(func)
{
    req_ = std::make_shared<request>();
    parser_ = std::make_shared<http_parser>();
}

tcp_session::~tcp_session()
{
    close();
}

void tcp_session::run()
{
    set_no_delay();
    async_read();
    active_ = true;
}

void tcp_session::close()
{
    active_ = false;
    if (socket_.is_open())
    {
        boost::system::error_code ignore_ec;
        socket_.shutdown(boost::asio::socket_base::shutdown_both, ignore_ec);
        socket_.close(ignore_ec);
    }
}

boost::asio::io_service& tcp_session::get_io_service()
{
    return ios_;
}

boost::asio::ip::tcp::socket& tcp_session::get_socket()
{
    return socket_;
}

void tcp_session::async_write(const std::shared_ptr<std::string>& network_data)
{
    if (!active_)
    {
        return;
    }

    auto self(shared_from_this());
    ios_.post([this, self, network_data]
    {
        bool empty = send_queue_.empty();
        send_queue_.emplace_back(network_data);
        if (empty)
        {
            async_write_loop();
        }
    });
}

void tcp_session::async_write_loop()
{
    auto self(shared_from_this());
    boost::asio::async_write(socket_, boost::asio::buffer(*send_queue_.front()), 
                             [this, self](boost::system::error_code ec, std::size_t)
    {
        if (!ec)
        {
            send_queue_.pop_front();
            if (!send_queue_.empty())
            {
                async_write_loop();
            }
        }
        else
        {
            log_warn << ec.message();
            send_queue_.clear();
        }
    });
}

void tcp_session::async_read()
{
    auto self(shared_from_this());
    socket_.async_read_some(boost::asio::buffer(buffer_),
                            [this, self](const boost::system::error_code& ec, std::size_t bytes_transferred)
    {
        if (!ec)
        {
            parse_result ret = parser_->parse(req_, buffer_.data(), buffer_.data() + bytes_transferred);
            if (ret == parse_result::finished)
            {
                func_(req_, std::make_shared<response>(self));
            }
            else if (ret == parse_result::error)
            {
                async_write(parser_->pack(status_type::bad_request, "Bad Request"));
            }
            else
            {
                async_read();
            }
        }
        else if (active_ && ec != boost::asio::error::operation_aborted)
        {
            close();
        }
    });
}

void tcp_session::set_no_delay()
{
    boost::asio::ip::tcp::no_delay option(true);
    boost::system::error_code ec;
    socket_.set_option(option, ec);
}
