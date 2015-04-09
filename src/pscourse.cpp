#include "pscourse.h"

pscourse::pscourse(const string_type & url)
{
}

const pscourse::string_type pscourse::get_title() const
{
    return title_;
};

void pscourse::set_title(const string_type title)
{
    for(auto && c : title)
        title_ += isspace(c) ? '-' : tolower(c);
};

const pscourse::string_type pscourse::get_author() const
{
    return author_;
};

void pscourse::set_author(const string_type author)
{
    author_ = author;
};

void pscourse::add(psmodule & module)
{
    modules_.push_back(module);
};