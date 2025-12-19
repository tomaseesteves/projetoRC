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

string handle_request(string full_msg)
{
    string request = full_msg.substr(0, 3);
    trim(full_msg);
    string msg;

    switch (resolve_server_request(request))
    {
    case login:
    {
        return handle_login(split_string(full_msg));
    }
    case changePass:
    {
        return handle_changePass(split_string(full_msg));
    }
    case unregister:
    {
        return handle_unregister(split_string(full_msg));
    }
    case logout:
    {
        return handle_logout(split_string(full_msg));
    }
    case create:
    {
        return handle_create(divide_create_request(full_msg));
    }
    case close_event:
    {
        return handle_close_event(split_string(full_msg));
    }
    case myevents:
    {
        return handle_myevents(split_string(full_msg));
    }
    case list:
    {
        return handle_list(split_string(full_msg));
    }
    case show:
    {
        return handle_show(split_string(full_msg));
    }
    case reserve:
    {
        return handle_reserve(split_string(full_msg));
    }
    case myreservations:
    {
        return handle_myreservations(split_string(full_msg));
    }
    default:
    {
        msg = "ERR\n";
        return msg;
    }
    }
}
