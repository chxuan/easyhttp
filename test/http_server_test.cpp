#include "http_server_test.h"

using namespace std::placeholders;

http_server_test::http_server_test()
{
    server_ = std::make_shared<http_server>("0.0.0.0:6666", 4, 4);
    server_->bind("/add", std::bind(&http_server_test::deal_add, this, _1, _2));
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

void http_server_test::deal_add(const std::shared_ptr<request>& req, const std::shared_ptr<response>& res)
{
    int a = std::atoi(req->get_param("a").c_str());
    int b = std::atoi(req->get_param("b").c_str());
    res->set_response(std::to_string(a + b));
}
