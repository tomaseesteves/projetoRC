#include <iostream>
#include <vector>
#include <iterator>
#include <sstream>
#include <ctime>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <iomanip>

#include <utils.hpp>
#include <constants.hpp>

using namespace std;

bool check_size_uid(string &uid)
{
    return uid.size() == 6;
}

bool check_size_eid(string &eid)
{
    return eid.size() == 3;
}

bool check_size_password(string &password)
{
    return password.size() == 8;
}

bool check_size_file_name(string &s)
{
    return s.size() <= 24;
}

bool check_size_event_name(string &s)
{
    return s.size() <= 10;
}

bool check_size_file(int file_size)
{
    return file_size < MAX_FILE_SIZE;
}

bool check_only_digits(string &s)
{
    for (char c : s)
    {
        if (!isdigit(c))
        {
            return false;
        }
    }
    return true;
}

bool check_only_alphanumerical(string &s)
{
    for (char c : s)
    {
        if (!isdigit(c) && !isalpha(c))
        {
            return false;
        }
    }
    return true;
}

bool check_date(string &s)
{
    tm tm = {};
    istringstream ss(s);

    ss >> get_time(&tm, "%d-%m-%Y");

    if (ss.fail())
    {
        return false;
    }

    return true;
}

bool check_hour(string &s);

bool check_is_future_time(string &s, string &s2);

bool check_file_name(string &s)
{
    // Find last occurence of '.'
    const char *dot = strrchr(s.c_str(), '.');

    // check for valid extension?
    if (!dot || strlen(dot) != 4)
    {
        return false;
    }

    for (char c : s)
    {
        if (!isdigit(c) && !isalpha(c) && c != '-' && c != '_' && c != '.')
        {
            cout << "INVALID CHAR\n";
            return false;
        }
    }

    return true;
}