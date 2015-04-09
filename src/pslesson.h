#ifndef PSLESSON_H
#define PSLESSON_H

#include <string>

class pslesson
{
public:
    typedef std::string string_type;

    pslesson(const string_type & url);

private:
    std::string title_;
    std::string author_;
};

#endif