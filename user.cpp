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

using namespace std;

#define PORT "58044"
#define IP "127.0.0.0" 
#define MAX_STRING 128

int fd, errcode;
ssize_t msg;
socklen_t addrlen;
struct addrinfo hints, *res;
struct sockaddr_in addr;
string port, ip_address;

enum Command_Options {
    login,
    changePass,
    unregister,
    logout,
    exit_user,
    create,
    close_reservation,
    myevents,
    list, 
    show, 
    reserve,
    myreservations,
    invalid_command
};

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
    
    return invalid_command;
}

vector<string> splitString(string& input)
{
    istringstream stream(input);
    vector<string> tokens;
    string token;
    char delimiter = ' ';

    while (getline(stream, token, delimiter)) {
        tokens.push_back(token);
    }
    return tokens;
}

string connect_UDP(string ip_address, string port, string msg) {
    fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (fd == -1) {
        cout << "Erro a criar socket.\n";
        exit(1);
    }

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_DGRAM;

    errcode = getaddrinfo(ip_address.c_str(), port.c_str(), &hints, &res);
    if (errcode != 0) {
        cout << "Erro a estabelecer conexão UDP.\n";
        exit(1);
    }

    int msg_len = msg.length();

    ssize_t sent = sendto(fd, msg.c_str(), msg_len, 0, res->ai_addr, res->ai_addrlen);
    if (sent == -1) {
        cout << "Erro a enviar mensagem para servidor UDP.\n";
        exit(-1);
    }

    char buffer[MAX_STRING];
    addrlen = sizeof(addr);
    sent = recvfrom(fd, buffer, MAX_STRING, 0, 
                   (struct sockaddr*) &addr, &addrlen);
    if (sent == -1) {
        cout << "Erro a receber mensagem do servidor UDP.\n";
        exit(-1);
    }

    write(1, buffer, sent);  // só para testar
    string response = buffer;

    freeaddrinfo(res);
    close(fd);

    return response;
}

int main(int argc, char** argv) {
    bool exit_program = false, logged_in = false;
    string input, command, response, msg;

    // Use default values for IP address and port
    if (argc == 1) {
        ip_address = IP;
        port = PORT;
    }
    // Use IP and port values given by user
    else {
        ip_address = argv[1];
        port = argv[2];
    }        

    while(!exit_program) {
        // Read input from user and extract first word 
        
        getline(cin, input);
        vector<string> tokens = splitString(input);
        command = tokens[0];

        switch (resolveCommand(command))
        {
            case login: {
                if (tokens.size() == 1) {
                    cout << "Por favor introduza o seu username e password.\n";
                    break;
                }

                msg = "LIN" + ' ' + tokens[1] + ' ' + tokens[2] + '\n';
                response = connect_UDP(ip_address, port, msg);
                string status = splitString(response)[1];

                if (status == "REG") {
                    cout << "Conta criada com sucesso.\n";
                    logged_in = true;
                    break;
                }
                else if (status == "NOK") {
                    cout << "Username ou password incorretos.\n";
                    break;
                }
                cout << "Login efetuado com sucesso.\n";
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
                else exit_program = true;
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
        }
    }

    return 0; 
}