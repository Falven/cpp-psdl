#ifndef PSCLIENT_H
#define PSCLIENT_H

#include <stdio.h>
#include <string>
#include <vector>
#include <type_traits>
#include <utility>

#include <Poco/URI.h>
#include <Poco/Path.h>
#include <Poco/File.h>

#include <curl/curl.h>
#include <gumbo-query/Document.h>

#include "pscourse.h"
#include "psmodule.h"
#include "pslesson.h"

class psclient
{
public:
    typedef std::string string_type;
    typedef string_type::value_type value_type;
    typedef string_type::const_iterator const_iterator;

    typedef Poco::URI uri_type;
    typedef Poco::File file_type;
    typedef Poco::Path path_type;

    // Constructor
    psclient(const string_type & username, const string_type & password);

    // Copy constructor
    psclient(const psclient & other) = delete;

    // Move constructor
    psclient(psclient && other) = default;

    // Copy assignment
    psclient & operator=(const psclient & other) = delete;

    // Move assignment operator
    psclient & operator=(psclient && other) = default;

    /*
     *  Authenticates the provided user and password against the ps system.
     */
    void authenticate(const string_type & user, const string_type & pass);

    /*
     * Searches Pluralsight for items matching the provided text.
     */
    std::vector<string_type> search(const string_type & text);

    /*
     *  Downloads the provided course to the given directory.
     */
    void download(const pscourse & course,
                  const string_type & course_dir = path_type::current());

    /*
     *  Downloads the provided module to the given directory.
     */
    void download(const psmodule & module,
                  const string_type & module_dir = path_type::current());

    /*
     *  Downloads the provided lesson to the given directory.
     */
    void download(const pslesson & lesson,
                  const string_type & lesson_dir = path_type::current());

    void deleter(CURL * ptr_curl);

private:
    uri_type auth_;

    char error_buffer[CURL_ERROR_SIZE];

    CURL * init();

    // uptr with function ptr
    std::unique_ptr<CURL, std::function<void(CURL *)>> uptr_curl_;

    void authentication_init(CURL * conn, CURLcode & result);

    static int process_auth_get(char * ptr_data,
                                size_t size,
                                size_t nmemb,
                                string_type * get_buffer);

    string_type authentication_get(CURL * conn, CURLcode & result);

    void authentication_post(CURL * conn,
                             const string_type & data,
                             CURLcode & result);

    string_type parse_veri_token(CDocument & doc);

    string_type parse_user_token(CDocument & doc);

    string_type parse_pass_token(CDocument & doc);

    string_type file_get(const path_type & path);
};

#endif