#include "http_server_test.h"
#include <fstream>

using namespace std::placeholders;

http_server_test::http_server_test()
{
    server_ = std::make_shared<easyhttp::http_server>("0.0.0.0:6666", 4, 4);
    server_->route("/add", std::bind(&http_server_test::deal_add, this, _1, _2));
    server_->route("/download", std::bind(&http_server_test::deal_download, this, _1, _2));
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

void http_server_test::deal_add(const std::shared_ptr<easyhttp::request>& req, 
                                const std::shared_ptr<easyhttp::response>& res)
{
    int a = std::atoi(req->get_param_value("a").c_str());
    int b = std::atoi(req->get_param_value("b").c_str());
    log_info << "a + b = " << a + b;
    res->set_response(std::to_string(a + b));
}

void http_server_test::deal_download(const std::shared_ptr<easyhttp::request>& req, 
                                     const std::shared_ptr<easyhttp::response>& res)
{
    std::string file_name = req->get_param_value("file_name");
    std::ifstream file(file_name, std::ios::binary);
    if (!file)
    {
        res->set_response(easyhttp::status_type::not_found);
        log_warn << "Not find file name: " << file_name;
        return;
    }

    static const int max_buf_size = 5 * 1024 * 1024;
    char* buf = new char[max_buf_size];

    file.read(buf, max_buf_size);
    log_info << file_name << " file size: " << file.gcount();
    file.close();

    res->set_response(std::string(buf, file.gcount()));
    delete[] buf;
}
