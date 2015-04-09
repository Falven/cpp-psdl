#include <iostream>
#include <exception>
#include <algorithm>
#include <cstring>
#include <memory>
#include <fstream>
#include <functional>

#include <Poco/URI.h>
#include <Poco/Path.h>
#include <Poco/File.h>
#include <curl/curl.h>

#include <gumbo-query/Document.h>
#include <gumbo-query/Node.h>

#include "psclient.h"
#include "curl_error.h"

#define AUTH_URL "https://www.pluralsight.com/a/SignIn"

/**
 * @brief initializes curl
 * @details [long description]
 * 
 * @return a pointer to the initialized curl
 */
CURL * psclient::init()
{
    CURLcode result = curl_global_init(CURL_GLOBAL_DEFAULT);
    if(result != CURLE_OK)
        throw curl_error(result, "Failed to create CURL connection.");
    return curl_easy_init();
}

/**
 * @brief Deleter function for curl unique_pointer.
 * @details [long description]
 * 
 * @param ptr_curl pointer to curl to use for deletion.
 */
void psclient::deleter(CURL * ptr_curl)
{
    curl_easy_cleanup(ptr_curl);
    curl_global_cleanup();
}

/**
 * Construct an instance of psclient using the
 * DEFAULT_AUTH uri for user authentication;
 *
 * @throw curl_error If there was a problem initializing curl.
 */
psclient::psclient(const string_type & username, const string_type & password)
: auth_(AUTH_URL)
, uptr_curl_(init(),
             [this](CURL * ptr_curl) -> void
             {
                 this->deleter(ptr_curl);
             })
{
    try
    {
        CURL * conn = uptr_curl_.get();
        CURLcode result;

        authentication_init(conn, result);

        string_type get_buffer = authentication_get(conn, result);
        string_type get_buffer_raw = get_buffer.c_str();

        CDocument doc;
        doc.parse(get_buffer_raw);

        string_type post_buffer;
        post_buffer += parse_veri_token(doc);

        post_buffer += parse_user_token(doc);
        post_buffer +=
            curl_easy_escape(conn, username.c_str(), (int)username.length());

        post_buffer += parse_pass_token(doc);
        post_buffer +=
            curl_easy_escape(conn, password.c_str(), (int)password.length());

        authentication_post(conn, post_buffer, result);
    }
    catch(...)
    {
        throw;
    }
}

void psclient::authentication_init(CURL * conn, CURLcode & result)
{
    result = curl_easy_setopt(conn, CURLOPT_URL, auth_.toString().c_str());
    if(result != CURLE_OK)
        throw curl_error(result, "Failed to set authentication URL.");

    result = curl_easy_setopt(conn, CURLOPT_SSL_VERIFYPEER, 1L);
    if(result != CURLE_OK)
        throw curl_error(result, "Failed to set CURLOPT_SSL_VERIFYPEER.");

    result = curl_easy_setopt(conn, CURLOPT_COOKIEFILE, "");
    if(result != CURLE_OK)
        throw curl_error(result, "Failed to set CURLOPT_COOKIEFILE");

    result = curl_easy_setopt(conn, CURLOPT_FOLLOWLOCATION, 1L);
    if(result != CURLE_OK)
        throw curl_error(result, "Failed to set redirect option.\n");
}

// This callback function gets called by libcurl as soon as there is data
// received that needs to be saved. ptr points to the delivered data,
// and the size of that data is size multiplied with nmemb.
int psclient::process_auth_get(char * ptr_data,
                               size_t size,
                               size_t nmemb,
                               std::string * get_buffer)
{
    if(get_buffer == NULL)
        return 0;

    size_t data_size = size * nmemb;
    get_buffer->append(ptr_data, data_size);

    return (int)data_size;
}

psclient::string_type psclient::authentication_get(CURL * conn,
                                                   CURLcode & result)
{
    string_type get_buffer;

    result = curl_easy_setopt(
        conn, CURLOPT_WRITEFUNCTION, &psclient::process_auth_get);
    if(result != CURLE_OK)
        throw curl_error(result, "Failed to set data writer.");

    result = curl_easy_setopt(conn, CURLOPT_WRITEDATA, &get_buffer);
    if(result != CURLE_OK)
        throw curl_error(result, "Failed to set write data.");

    result = curl_easy_perform(conn);
    if(result != CURLE_OK)
        throw curl_error(result, "Failed to perform authentication.");

    return get_buffer;
}

void psclient::authentication_post(CURL * conn,
                                   const string_type & data,
                                   CURLcode & result)
{
    string_type get_buffer;

    result = curl_easy_setopt(conn, CURLOPT_READDATA, &data);
    if(result != CURLE_OK)
        throw curl_error(result, "Failed to set read data.");

    result = curl_easy_setopt(conn, CURLOPT_WRITEDATA, &get_buffer);
    if(result != CURLE_OK)
        throw curl_error(result, "Failed to set write data.");

    result = curl_easy_perform(conn);
    if(result != CURLE_OK)
        throw curl_error(result, "Failed to perform authentication.");
}

psclient::string_type psclient::parse_veri_token(CDocument & doc)
{
    CSelection c = doc.find("form[action=\'" AUTH_URL "\'");
    for(unsigned int i = 0; i < c.nodeNum(); i++)
    {
        CNode n = c.nodeAt(i);
        CNode a = n.childAt(0);
        std::cout << a.attribute("href") << a.text() << std::endl;
    }

    return string_type();
}

psclient::string_type psclient::parse_user_token(CDocument & doc)
{
    return string_type();
}

psclient::string_type psclient::parse_pass_token(CDocument & doc)
{
    return string_type();
}

/**
 * Returns the contents of the file denoted from the
 * provided path as a string_type.
 *
 * @param path Patht to the file whose contents to read.
 * @throw std::ifstream::failure if the provided path is invalid.
 * @return string_type containing the contents of the read file.
 */
psclient::string_type psclient::file_get(const path_type & path)
{
    // Stream to file (Automatically closes in destructor).
    std::ifstream ifs(path.toString(),
                      std::ios_base::in | std::ios_base::binary);

    // Enable exceptions
    ifs.exceptions();

    // Set ifs input position indicator to end.
    ifs.seekg(0, std::ios::end);
    // Record size of ifs buffer.
    std::ifstream::pos_type size = ifs.tellg();
    // Set ifs input position indicator to beginning.
    ifs.seekg(0);

    // Init string buffer to hold file data.
    string_type buffer(size, ' ');
    // Read file contents into string buffer.
    ifs.read(&buffer[0], size);

    // Return by RVO (Reurn value optimization).
    return buffer;
}

/**
 * Authenticates the user to be able to access server resources.
 *
 * @param user The username of the user to authenticate.
 * @param pass The password of the user to authenticate.
 * @throw std::invalid_argument if the provided user or password is invalid.
 */
void psclient::authenticate(const string_type & user, const string_type & pass)
{
    // Reponse code.
    CURLcode result;

    // URL-encode.
    string_type url_str = auth_.toString();

    // Pass in a pointer to the URL to work with. The parameter should be
    // a char * to a zero terminated string which must be URL-encoded.
    curl_easy_setopt(uptr_curl_.get(),
                     CURLOPT_URL,
                     curl_easy_escape(uptr_curl_.get(),
                                      url_str.c_str(),
                                      (int)(url_str.length())));

    // Prepare get request.
    string_type get_buffer;
    curl_easy_setopt(uptr_curl_.get(), CURLOPT_READDATA, &get_buffer);

    // Perform the request, res will get the return code
    result = curl_easy_perform(uptr_curl_.get());

    // Prepare post request.
    //    string_type post_buffer;
    //    curl_easy_setopt(&ptr_curl_, CURLOPT_WRITEDATA, &post_buffer);

    // Check for errors
    if(result != CURLE_OK)
        throw curl_error(result);
}

std::vector<psclient::string_type> psclient::search(const string_type & text)
{
    return std::vector<string_type>();
}

/*
 *  Downloads the provided course to the given directory.
 */
void psclient::download(const pscourse & course, const string_type & course_dir)
{
    path_type course_path;
    course_path.parse(course_dir);
    file_type course_file(course_path);
    course_file.createDirectory();
}

/*
 *  Downloads the provided module to the given directory.
 */
void psclient::download(const psmodule & module, const string_type & module_dir)
{
}

/*
 *  Downloads the provided lesson to the given directory.
 */
void psclient::download(const pslesson & lesson, const string_type & lesson_dir)
{
}
