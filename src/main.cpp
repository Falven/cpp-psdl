#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <ncurses.h>

#include "psclient.h"
#include "pscourse.h"
#include "psmodule.h"
#include "pslesson.h"
#include "network_error.h"
#include "curl_error.h"

#define VERSION "1.0.0.0"

using namespace std;

static void print_title()
{
    cout << R"(
A pluralsight video downloading utility.
Requires a valid Pluralsight.com username, password, and subscription.
    )" << endl;
}

static void print_help_usage()
{
    cout << R"(General usage:
    invoke  info    option1     option2     arg1    arg2
    psdl    [help]
    psdl    [help]  search                  TEXT
    psdl    [help]  download    course      URL     [DEST]
    psdl    [help]  download    module      URL     [DEST]
    psdl    [help]  download    lesson      URL     [DEST]
    psdl    [help]  version
    )" << endl;
}

static void print_search_usage()
{
    cout << R"(
search:
    
    psdl    [help]  search TEXT
    
    searches pluralsight for the provided text,
    printing out the names and URL's of matching courses.
    )" << endl;
}

static void print_download_usage()
{
    cout << R"(
download:
    
    psdl    [help]  download    course      URL     [DEST]
    psdl    [help]  download    module      URL     [DEST]
    psdl    [help]  download    lesson      URL     [DEST]
    
    download a Pluralsight course, module or lesson
    from the given URL, optionally, to the given destination directory,
    oherwise to the current directory.
    )" << endl;
}

static int print_error_unknown()
{
    cerr << "An unknown error has occurred." << endl;
    return 1;
}

static void print_error_option_unknown(const string arg)
{
    cerr << "Unknown option provided.\n"
         << "psdl\t\thelp\t\t" << arg << endl;
}

static int print_error_net(curl_error ce)
{
    cerr << "There was a problem connecting to Pluralsight.\n" << ce.what()
         << endl;
    return 1;
}

static string prompt_username()
{
    string userbuff;
    cout << "Pluralsight Username: ";
    cin >> userbuff;
    return userbuff;
}

static string prompt_password()
{
    initscr(); // enable ncurses

    cout << "Password: ";

    noecho(); // disable character echoing

    char pwdbuff[64];
    getnstr(pwdbuff, sizeof(pwdbuff));
    getch(); // Wait for a keypress

    echo(); // enable character echoing again

    endwin(); // disable ncurses

    return pwdbuff;
}

static int parse_version_option(const int & argc, char ** argv)
{
    cout << "Pluralsight Downloader, version: " << VERSION << endl;
    return 0;
}

static int parse_help_option(const int & argc, char ** argv)
{
    if(argc == 2)
    {
        print_title();
        print_help_usage();
        return 0;
    }
    else
    {
        string arg2(argv[2]);
        if(arg2 == "search")
        {
            print_search_usage();
            return 0;
        }
        else if(arg2 == "download")
        {
            print_download_usage();
            return 0;
        }
        else if(arg2 == "version")
        {
            return parse_version_option(argc, argv);
        }
        else
        {
            print_error_option_unknown(arg2);
            print_help_usage();
        }
    }
    return print_error_unknown();
}

static int parse_search_option(const int & argc, char ** argv)
{
    if(argc > 2)
    {
        try
        {
            string arg2(argv[2]);
            psclient client("falven2000@hotmail.com", "Franramak1!");
            //            client.authenticate(prompt_username(),
            //            prompt_password());
            // TODO make sure RVO works here. No copy.
            vector<string> results = client.search(arg2);
            for(auto && result : results)
                cout << "\n" << result;
            cout << endl;
            return 0;
        }
        catch(curl_error ce)
        {
            return print_error_net(ce);
        }
    }
    return print_error_unknown();
}

static int parse_download_option(const int & argc, char ** argv)
{
    if(argc >= 4)
    {
        string arg2(argv[2]);
        string arg3(argv[3]);
        try
        {
            psclient client("falven2000@hotmail.com", "Franramak1!");
            //            client.authenticate(prompt_username(),
            //            prompt_password());

            if(arg2 == "course")
            {
                pscourse course(arg3);
                argc == 5 ? client.download(course, argv[4])
                          : client.download(course);
            }
            else if(arg2 == "module")
            {
                psmodule module(arg3);
                argc == 5 ? client.download(module, argv[4])
                          : client.download(module);
            }
            else if(arg2 == "lesson")
            {
                pslesson lesson(arg3);
                argc == 5 ? client.download(lesson, argv[4])
                          : client.download(lesson);
            }
            else
            {
                print_error_option_unknown(arg2);
                print_download_usage();
                return 1;
            }
        }
        catch(curl_error ce)
        {
            return print_error_net(ce);
        }
    }
    else
    {
        cerr << "Missing URL argument." << endl;
        print_download_usage();
        return 1;
    }
    return 0;
}

int main(int argc, char * argv[])
{
    if(argc > 1)
    {
        string arg1(argv[1]);
        if(arg1 == "help")
        {
            return parse_help_option(argc, argv);
        }
        else if(arg1 == "search")
        {
            return parse_search_option(argc, argv);
        }
        else if(arg1 == "download")
        {
            return parse_download_option(argc, argv);
        }
        else if(arg1 == "version")
        {
            return parse_version_option(argc, argv);
        }
        else
        {
            print_error_option_unknown(arg1);
            print_help_usage();
            return 1;
        }
    }
    else
    {
        print_title();
        print_help_usage();
        return 0;
    }
}