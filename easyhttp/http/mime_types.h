/**
 * @file mime_types.h
 * @brief mime类型映射关系
 * @author chxuan, 787280310@qq.com
 * @version 1.0.0
 * @date 2018-01-07
 */
#pragma once

#include <string>
#include <unordered_map>

namespace easyhttp 
{

class mime_types
{
public:
    static std::string get_mime_type(const std::string& extension);

private:
    static std::unordered_map<std::string, std::string> mime_type_map_;    
};

}
