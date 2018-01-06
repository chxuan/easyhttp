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
    return tcp_server::run();
}

void http_server::stop()
{
    tcp_server::stop();
}
