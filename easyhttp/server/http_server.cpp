#include "http_server.h"
#include "easyhttp/utility/logger.h"

http_server::http_server(const std::string& host, int ios_threads, int work_threads)
    : tcp_server(host, ios_threads), 
    work_threads_(work_threads)
{

}

http_server::~http_server()
{
    stop();
}

bool http_server::run()
{
    if (router_.route_table_size() == 0)
    {
        log_warn << "Route table is empty";
        return false;
    }

    if (tcp_server::run())
    {
        router_.run(work_threads_);
        return true;
    }

    return false;
}

void http_server::stop()
{
    tcp_server::stop();
    router_.stop();
}

void http_server::route(const std::string& uri, const request_handler& func)
{
    router_.route(uri, func);
}

void http_server::deal_request(const std::shared_ptr<request>& req, const std::shared_ptr<response>& res)
{
    router_.do_route(req, res);
}
