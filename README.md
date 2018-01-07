A modern C++ http framework
===============================================

> easyhttp是采用C++开发的，使用方便的http server库。

![License][1] 

## Tutorial

* **Simple server**

    ```cpp
    #include "easyhttp/easyhttp.h"
    
    using namespace std::placeholders;

    void add(const std::shared_ptr<request>& req, const std::shared_ptr<response>& res)
    {
        int a = std::atoi(req->get_param("a").c_str());
        int b = std::atoi(req->get_param("b").c_str());
        res->set_response(std::to_string(a + b));
    }

    int main()
    {
        // 1.创建http server服务对象
        // 服务端将采用1个io线程和2个work线程服务
        auto server = std::make_shared<http_server>("0.0.0.0:6666", 1, 2);

        // 2.绑定add函数
        server->bind("/add", std::bind(add, _1, _2));
    
        // 3.启动事件循环（非阻塞）
        server->run();

        std::cin.get();
        return 0;
    }
    ```
    
## 开发平台

* Ubuntu17.10 gcc7.2.0

## 依赖性

* boost
* c++11

## DONE

* HTTP1.0。

## TODO

* HTTP1.1的支持。

## License
This software is licensed under the [MIT license][2]. © 2018 chxuan


  [1]: http://img.shields.io/badge/license-MIT-blue.svg?style=flat-square
  [2]: https://github.com/chxuan/easyrpc/blob/master/LICENSE
