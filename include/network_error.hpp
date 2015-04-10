#ifndef NETWORK_ERROR_H
#define NETWORK_ERROR_H

#include <stdexcept>
#include <string>

class network_error : public std::runtime_error
{
public:
    inline explicit network_error(const std::string & what_arg)
    : runtime_error(what_arg){};

    inline explicit network_error(const char * what_arg)
    : runtime_error(what_arg){};
};

#endif