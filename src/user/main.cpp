#include <iostream>
#include <vector>
#include <iterator>
#include <sstream>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#include <parser.hpp>
#include <network.hpp>
#include <constants.hpp>
#include <protocol.hpp>

using namespace std;

int main(int argc, char **argv)
{
    bool exit_program = false;
    string input, command;
    vector<string> tokens;

    handle_ip_port(argc, argv);

    while (!exit_program)
    {
        // Read input from user and extract first word

        getline(cin, input);
        tokens = splitString(input);
        command = tokens[0];

        switch (resolveCommand(command))
        {
        case login:
        {
            handle_login(tokens);
            break;
        }
        case changePass:
        {
            handle_changePass(tokens);
            break;
        }
        case unregister:
        {
            handle_unregister(tokens);
            break;
        }
        case logout:
        {
            handle_logout(tokens);
            break;
        }
        case exit_user:
        {
            if (!handle_exit_user(tokens))
                exit_program = true;
            break;
        }
        case create:
        {
            handle_create(tokens);
            break;
        }
        case close_event:
        {
            handle_close_event(tokens);
            break;
        }
        case myevents:
        {
            handle_myevents(tokens);
            break;
        }
        case list:
        {
            handle_list(tokens);
            break;
        }
        case show:
        {
            handle_show(tokens);
            break;
        }
        case reserve:
        {
            handle_reserve(tokens);
            break;
        }
        case myreservations:
        {
            handle_myreservations(tokens);
            break;
        }

        /**
         * Invalid commands.
         */
        default:
            cout << "Apenas são autorizados comandos válidos.\n";
            break;
        }
    }

    return 0;
}