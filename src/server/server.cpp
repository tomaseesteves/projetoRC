#include <sys/types.h>
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

#include <protocol.hpp>
#include <parser.hpp>
#include <vector>
#include <string>

using namespace std;

int main(int argc, char **argv)
{
    string s, msg;
    vector<string> request;
    request = splitString(s);
    handle_ip_port(argc, argv);
    while (true)
    {
        switch (resolveServerRequest(1))
        {
        case login:
        {
            msg = handle_login(request);
            break;
        }
        case changePass:
        {
            msg = handle_changePass(request);
            break;
        }
        case unregister:
        {
            msg = handle_unregister(request);
            break;
        }
        case logout:
        {
            msg = handle_logout(request);
            break;
        }
        case create:
        {
            msg = handle_create(request);
            break;
        }
        case close_event:
        {
            msg = handle_close_event(request);
            break;
        }
        case myevents:
        {
            msg = handle_myevents(request);
            break;
        }
        case list:
        {
            msg = handle_list(request);
            break;
        }
        case show:
        {
            msg = handle_show(request);
            break;
        }
        case reserve:
        {
            msg = handle_reserve(request);
            break;
        }
        case myreservations:
        {
            msg = handle_myreservations(request);
            break;
        }
        case invalid_command:
        {
            msg = "ERR\n";
            break;
        }
        }
    }
    return 0;
}
