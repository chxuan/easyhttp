#include "http_server_test.h"

http_server_test::http_server_test()
{
    server_ = std::make_shared<http_server>("0.0.0.0:6666", 4, 4);
}

http_server_test::~http_server_test()
{
    stop();
}

void http_server_test::run()
{
    bool ok = server_->run();
    if (!ok)
    {
        log_error << "http server start failed";
        return;
    }

    log_info << "http server start...";
}

void http_server_test::stop()
{
    server_->stop();
}

