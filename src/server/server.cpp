#include <sys/types.h>
#include <iostream>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <time.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#define MYPORT "58000"

#include <protocol_server.hpp>
#include <parser.hpp>
#include <vector>
#include <string>

using namespace std;

bool vebose = false;

string handle_request(string full_msg)
{
    string request = full_msg.substr(0, 3);
    trim(full_msg);
    string msg;

    switch (resolve_server_request(request))
    {
    case login:
    {
        if (vebose)
        {
            cout << "Login command received; ";
        }
        return handle_login(split_string(full_msg));
    }
    case changePass:
    {
        if (vebose)
        {
            cout << "Changepass command received; ";
        }
        return handle_changePass(split_string(full_msg));
    }
    case unregister:
    {
        if (vebose)
        {
            cout << "Unregister command received; ";
        }
        return handle_unregister(split_string(full_msg));
    }
    case logout:
    {
        if (vebose)
        {
            cout << "Logout command received; ";
        }
        return handle_logout(split_string(full_msg));
    }
    case create:
    {
        if (vebose)
        {
            cout << "Create command received; ";
        }
        return handle_create(divide_create_request(full_msg));
    }
    case close_event:
    {
        if (vebose)
        {
            cout << "Close command received; ";
        }
        return handle_close_event(split_string(full_msg));
    }
    case myevents:
    {
        if (vebose)
        {
            cout << "Myevents command received; ";
        }
        return handle_myevents(split_string(full_msg));
    }
    case list:
    {
        if (vebose)
        {
            cout << "List command received; ";
        }
        return handle_list(split_string(full_msg));
    }
    case show:
    {
        if (vebose)
        {
            cout << "Show command received; ";
        }
        return handle_show(split_string(full_msg));
    }
    case reserve:
    {
        if (vebose)
        {
            cout << "Reserve command received; ";
        }
        return handle_reserve(split_string(full_msg));
    }
    case myreservations:
    {
        if (vebose)
        {
            cout << "Myreservations command received; ";
        }
        return handle_myreservations(split_string(full_msg));
    }
    default:
    {
        msg = "ERR\n";
        if (vebose)
        {
            cout << "Invalid syntax received;\n";
        }
        return msg;
    }
    }
}
