#include <iostream>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

using namespace std;

<<<<<<< HEAD
#define PORT "58044"
#define IP "127.0.0.0" 
=======
#define PORT "58044" // mudar para 58000 + nº grupo
#define IPADDRESS "127.0.0.0"
>>>>>>> cdb2286bc14bdc86c9770b3cbf79293224e16efe

int fd, errcode;
ssize_t msg;
socklen_t addrlen;
struct addrinfo hints, *res;
struct sockaddr_in addr;
string port, ip_address;

enum Command_Options
{
    login,
    changePass,
    unregister,
    logout,
    exit_user,
    create,
<<<<<<< HEAD
    close_reservation,
=======
    closereservation,
>>>>>>> cdb2286bc14bdc86c9770b3cbf79293224e16efe
    myevents,
    list,
    show,
    reserve,
    myreservations,
    invalid_command
};

<<<<<<< HEAD
Command_Options resolveCommand(string command) {
    if( command == "login" ) return login;
    if( command == "changePass" ) return changePass;
    if( command == "unregister" ) return unregister;
    if( command == "logout" ) return logout;
    if( command == "exit" ) return exit_user;
    if( command == "create" ) return create;
    if( command == "close" ) return close_reservation;
    if( command == "myevents" or command == "mye" ) return myevents;
    if( command == "list" ) return list;
    if( command == "show" ) return show;
    if( command == "reserve" ) return reserve;
    if( command == "myreservations" or command == "myr") return myreservations;
    
=======
Command_Options resolveCommand(string command)
{
    if (command == "login")
        return login;
    if (command == "changePass")
        return changePass;
    if (command == "unregister")
        return unregister;
    if (command == "logout")
        return logout;
    if (command == "exit")
        return exit_user;
    if (command == "create")
        return create;
    if (command == "close")
        return closereservation;
    if (command == "myevents" or command == "mye")
        return myevents;
    if (command == "list")
        return list;
    if (command == "show")
        return show;
    if (command == "reserve")
        return reserve;
    if (command == "myreservations" or command == "myr")
        return myreservations;

>>>>>>> cdb2286bc14bdc86c9770b3cbf79293224e16efe
    return invalid_command;
}

void connect_UDP(string msg)
{
}

int main(int argc, char **argv)
{
    bool exit = false, logged_in = false;
    string input, command;

    // Use default values for IP address and port
    if (argc == 1)
    {
        ip_address = IPADDRESS;
        port = PORT;
    }
    // Use IP and port values given by user
    else
    {
        ip_address = argv[1];
        port = argv[2];
    }

    while (!exit)
    {
        // Read input from user and extract first word

        getline(cin, input);
        size_t found = input.find(" ");
        if (found != std::string::npos)
            command = input.substr(0, found);
        else
            command = input;

        switch (resolveCommand(command))
        {
<<<<<<< HEAD
            case login: {
                /* code */
                logged_in = true;
                break;
            }
            
            case changePass: {
                /* code */
                break;
            }
            
            case unregister: {
                /* code */
                logged_in = false;
                break;
            }
            
            case logout: {
                /* code */
                logged_in = false;
                break;
            }
            
            case exit_user: {
                if (logged_in) 
                    cout << "Please log out of your account before exiting.";
                else exit = true;
                break;
            }
            
            case create: {
                /* code */
                break;
            }
            
            case close_reservation: {
                /* code */
                break;
            }
            
            case myevents: {
                /* code */
                break;
            }
            
            case list: {
                /* code */
                break;
            }
            
            case show: {
                /* code */
                break;
            }
            
            case reserve: {
                /* code */
                break;
            }
            
            case myreservations: {
                /* code */
                break;
            }
            
            default:
                break;
=======
        case login:
        {
            /* code */
            logged_in = true;
            break;
        }

        case changePass:
        {
            /* code */
            break;
        }

        case unregister:
        {
            /* code */
            logged_in = false;
            break;
        }

        case logout:
        {
            /* code */
            logged_in = false;
            break;
        }

        case exit_user:
        {
            if (logged_in)
                cout << "Please log out of your account before exiting.";
            else
                exit = true;
            break;
        }

        case create:
        {
            /* code */
            break;
        }

        case closereservation:
        {
            /* code */
            break;
        }

        case myevents:
        {
            /* code */
            break;
        }

        case list:
        {
            /* code */
            break;
        }

        case show:
        {
            /* code */
            break;
        }

        case reserve:
        {
            /* code */
            break;
        }

        case myreservations:
        {
            /* code */
            break;
        }

        default:
            break;
>>>>>>> cdb2286bc14bdc86c9770b3cbf79293224e16efe
        }
    }

    return 0;
}