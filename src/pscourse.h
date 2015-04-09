#ifndef PSCOURSE_H
#define PSCOURSE_H

#include <string>
#include <vector>
#include "psmodule.h"

class pscourse
{
public:
    typedef std::string string_type;

    pscourse(const string_type & url);

    const string_type get_title() const;

    void set_title(const string_type title);

    const string_type get_author() const;

    void set_author(const string_type author);

    void add(psmodule & module);

private:
    string_type title_;
    string_type author_;
    std::vector<psmodule> modules_;
};

#endif