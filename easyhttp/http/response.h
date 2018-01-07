/**
 * @file response.h
 * @brief http应答
 * @author chxuan, 787280310@qq.com
 * @version 1.0.0
 * @date 2018-01-07
 */
#pragma once

#include <vector>
#include <memory>
#include "header.h"
#include "status_types.h"

class response
{
public:
    std::shared_ptr<std::string> pack(status_type type, const std::string& body);

private:
    std::vector<header> pack_header(int body_len);
    std::shared_ptr<std::string> make_network_data(status_type type, 
                                                   const std::vector<header>& headers, 
                                                   const std::string& body);
};

