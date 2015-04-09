#ifndef PSMODULE_H
#define PSMODULE_H

#include <string>
#include <vector>
#include "pslesson.h"

class psmodule
{
public:
    typedef std::string string_type;

    psmodule(const string_type & url);

private:
    std::string title_;
    std::string author_;
    std::vector<pslesson> lessons_;
};

#endif