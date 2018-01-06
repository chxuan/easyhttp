#include "tcp_session.h"
#include "easyhttp/utility/logger.h"
#include <iostream>

tcp_session::tcp_session(boost::asio::io_service& ios) 
    : ios_(ios), 
    socket_(ios)
{

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
#if 0
    resize_buffer(codec_->get_next_recv_bytes());
    auto self(shared_from_this());
    boost::asio::async_read(socket_, boost::asio::buffer(buffer_), 
                            [this, self](boost::system::error_code ec, std::size_t)
    {
        if (!ec)
        {
            codec_->decode(buffer_, self);
            async_read();
        }
        else if (active_ && ec != boost::asio::error::operation_aborted)
        {
            deal_connection_closed();
        }
    });
#endif
    auto self(shared_from_this());
    socket_.async_read_some(boost::asio::buffer(buffer_),
                            [this, self](const boost::system::error_code& ec, 
                                         std::size_t bytes_transferred)
    {
        if (!ec)
        {
            log_info << "bytes_transferred: " << bytes_transferred;
            log_info << buffer_.data();

            parse_result ret = parser_.parse(request_, buffer_.data(), buffer_.data() + bytes_transferred);
            if (ret == parse_result::finished)
            {
                std::cout << request_.method << std::endl;
                std::cout << request_.uri << std::endl;
                std::cout << request_.http_version_major << ", " << request_.http_version_minor << std::endl;
                for (auto& header : request_.headers)
                {
                    std::cout << header.name << ", " << header.value << std::endl;
                }
            }
        }
        else if (active_ && ec != boost::asio::error::operation_aborted)
        {
            deal_connection_closed();
        }

#if 0
    boost::tribool result;
    boost::tie(result, boost::tuples::ignore) = request_parser_.parse(
        request_, buffer_.data(), buffer_.data() + bytes_transferred);

    if (result)
    {
      request_handler_.handle_request(request_, reply_);
      boost::asio::async_write(socket_, reply_.to_buffers(),
          boost::bind(&connection::handle_write, shared_from_this(),
            boost::asio::placeholders::error));
    }
    else if (!result)
    {
      reply_ = reply::stock_reply(reply::bad_request);
      boost::asio::async_write(socket_, reply_.to_buffers(),
          boost::bind(&connection::handle_write, shared_from_this(),
            boost::asio::placeholders::error));
    }
    else
    {
      socket_.async_read_some(boost::asio::buffer(buffer_),
          boost::bind(&connection::handle_read, shared_from_this(),
            boost::asio::placeholders::error,
            boost::asio::placeholders::bytes_transferred));
    }
#endif
    });
}

void tcp_session::set_no_delay()
{
    boost::asio::ip::tcp::no_delay option(true);
    boost::system::error_code ec;
    socket_.set_option(option, ec);
}

void tcp_session::deal_connection_closed()
{
    close();
}

