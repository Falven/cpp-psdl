#ifndef CURL_ERROR_H
#define CURL_ERROR_H

#include <string>
#include <curl/curl.h>

#include "network_error.h"

class curl_error : public network_error
{
public:
    typedef std::string string_type;

    inline explicit curl_error(CURLcode error_code,
                               const string_type & what_arg = string_type())
    : error_code_(error_code)
    , error_msg_(curl_easy_strerror(error_code))
    , what_(what_arg)
    , network_error(what()){};

    inline virtual const char * what() const throw()
    {
        string_type ec_str = std::to_string((int)error_code_);
        return what_ == "" ? (ec_str + error_msg_).c_str()
                           : (ec_str + error_msg_ + ": " + what_).c_str();
    }

private:
    CURLcode error_code_;
    string_type error_msg_;
    string_type what_;
};

#endif