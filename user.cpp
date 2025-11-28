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
#define MAX_STRING 1024

int fd, errcode;
ssize_t msg;
socklen_t addrlen;
struct addrinfo hints, *res;
struct sockaddr_in addr;
char port[MAX_STRING], ip_address[MAX_STRING];

enum Command_Options
{
    login,
    changePass,
    unregister,
    logout,
    exit_user,
    create,
    close_event,
    myevents,
    list,
    show,
    reserve,
    myreservations,
    invalid_command
};

/**
 * Identify what command the user has just sent
 *
 * @param command
 * @return enum value
 */
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
        return close_event;
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

    return invalid_command;
}

/**
 * Splits given string into a vector, using ' ' as a delimiter.
 *
 * @param input
 * @return tokens
 */
vector<string> splitString(string &input)
{
    istringstream stream(input);
    vector<string> tokens;
    string token;
    char delimiter = ' ';

    while (getline(stream, token, delimiter))
    {
        tokens.push_back(token);
    }
    return tokens;
}

/**
 * Connection with UPD server.
 *
 * @param ip_address, port, msg
 * @return response
 */
string connect_UDP(char *ip_address, char *port, string msg)
{
    int msg_len = msg.length();
    char buffer[MAX_STRING];
    ssize_t sent;
    string response;

    fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (fd == -1)
    {
        cout << "Erro a criar socket.\n";
        exit(1);
    }

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_DGRAM;

    errcode = getaddrinfo(ip_address, port, &hints, &res);
    if (errcode != 0)
    {
        cout << "Erro a estabelecer conexão UDP.\n";
        exit(1);
    }

    sent = sendto(fd, msg.c_str(), msg_len, 0, res->ai_addr, res->ai_addrlen);
    if (sent == -1)
    {
        cout << "Erro a enviar mensagem para servidor UDP.\n";
        exit(-1);
    }

    addrlen = sizeof(addr);
    sent = recvfrom(fd, buffer, MAX_STRING, 0,
                    (struct sockaddr *)&addr, &addrlen);
    if (sent == -1)
    {
        cout << "Erro a receber mensagem do servidor UDP.\n";
        exit(-1);
    }

    write(1, buffer, sent); // só para testar
    response = buffer;

    freeaddrinfo(res);
    close(fd);

    return response;
}

/**
 * Connection with TCP server.
 *
 * @param ip_address, port, msg
 * @return response
 */
string connect_TCP(char *ip_address, char *port, string msg)
{
    int msg_len = msg.length();
    char buffer[MAX_STRING];
    ssize_t sent;
    string response;

    fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd == -1)
    {
        cout << "Erro a criar socket.\n";
        exit(1);
    }

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    errcode = getaddrinfo(ip_address, port, &hints, &res);
    if (errcode != 0)
    {
        cout << "Erro a estabelecer conexão TCP.\n";
        exit(1);
    }

    sent = connect(fd, res->ai_addr, res->ai_addrlen);
    if (sent == -1)
    {
        cout << "Erro a tentar conectar com servidor TCP.\n";
        exit(1);
    }

    sent = write(fd, msg.c_str(), msg_len);
    if (sent == -1)
    {
        cout << "Erro a enviar mensagem para servidor TCP.\n";
        exit(1);
    }

    sent = read(fd, buffer, MAX_STRING);
    if (sent == -1)
    {
        cout << "Erro a receber mensagem do servidor TCP.\n";
        exit(1);
    }

    write(1, buffer, sent); // só para testar
    response = buffer;

    freeaddrinfo(res);
    close(fd);

    return response;
}

int main(int argc, char **argv)
{
    bool exit_program = false, logged_in = false;
    string input, command, response, msg, curr_user = "", curr_pass = "", status;

    // Use default values for IP address and port
    if (argc == 1)
    {
        strcpy(ip_address, IP);
        strcpy(port, PORT);
    }
    // Use IP and port values given by user
    else
    {
        strcpy(ip_address, argv[1]);
        strcpy(port, argv[2]);
    }

    cout << "Bem vindo à nossa plataforma de reserva de eventos!\n";

    while (!exit_program)
    {
        // Read input from user and extract first word
        getline(cin, input);
        vector<string> tokens = splitString(input);
        command = tokens[0];

        switch (resolveCommand(command))
        {
        /**
         * Command: login UID password
         * Client message: LIN UID passwors
         * UDP server message: RLI status
         *
         * OK - successful login
         * REG - register new user
         * NOK - password doesn't match UID
         */
        case login:
        {
            if (tokens.size() != 3)
            {
                cout << "Por favor introduza o seu username e password.\n";
                break;
            }

            // Establish UDP connection
            msg = "LIN" + ' ' + tokens[1] + ' ' + tokens[2] + '\n';
            response = connect_UDP(ip_address, port, msg);

            status = splitString(response)[1];

            // Possible status outcomes
            if (status == "REG")
            {
                cout << "Conta criada com sucesso.\n";
                logged_in = true;
                break;
            }
            else if (status == "NOK")
            {
                cout << "Username ou password incorretos.\n";
                break;
            }

            cout << "Login efetuado com sucesso.\n";
            curr_user = tokens[1];
            curr_pass = tokens[2];
            logged_in = true;
            break;
        }

        /**
         * Command: changePass oldPassword newPassword
         * Client message: CPS UID oldPassword newPassword
         * TCP server message: RCP status
         *
         * OK - password changed successfully
         * NLG - user not logged in
         * NOK - old password doesn't match UID's current password
         * NID - UID doesn't exist
         */
        case changePass:
        {
            if (tokens.size() != 3)
            {
                cout << "Por favor introduza a password antiga e a password nova.\n";
                break;
            }

            // Establish TCP connection
            msg = "CPS" + ' ' + curr_user + tokens[1] + ' ' + tokens[2] + '\n';
            response = connect_TCP(ip_address, port, msg);

            status = splitString(response)[1];

            // Possible status outcomes
            if (status == "NLG")
            {
                cout << "Por favor faça login primeiro.\n";
                logged_in = true;
                break;
            }
            else if (status == "NOK")
            {
                cout << "Password dada não corresponde à password atual.\n";
                break;
            }
            else if (status == "NID")
            {
                cout << "Username fornecido não existe.\n";
                break;
            }

            cout << "Password alterada com sucesso.\n";
            curr_pass = tokens[2];
            break;
        }

        /**
         * Command: unregister
         * Client message: UNR UID password
         * UDP server message: RUR status
         *
         * OK - unregistered successfully
         * UNR - user was not registered
         * NOK - user was not logged in
         * WRP - incorrect password
         */
        case unregister:
        {
            if (tokens.size() != 1)
            {
                cout << "Apenas introduza o comando 'unregister'.\n";
                break;
            }

            // Establish UDP connection
            msg = "UNR" + ' ' + curr_user + ' ' + curr_pass + '\n';
            response = connect_UDP(ip_address, port, msg);

            status = splitString(response)[1];

            // Possible status outcomes
            if (status == "UNR")
            {
                cout << "Não pode remover o registo de uma conta não registada.\n";
                logged_in = true;
                break;
            }
            else if (status == "NOK")
            {
                cout << "Não pode remover o registo de uma conta que não se encontra ativa.\n";
                break;
            }
            else if (status == "WRP")
            {
                cout << "Password incorreta.\n";
                break;
            }

            // REMOVER PASTA DO USER!!!!!
            cout << "Unregister efetuado com sucesso.\n";
            curr_user = "";
            curr_pass = "";
            logged_in = false;
            break;
        }

        /**
         * Command: logout
         * Client message: LOU UID password
         * UDP server message: RLO status
         *
         * OK - logged out successfully
         * UNR - user was not registered
         * NOK - user was not logged in
         * WRP - incorrect password
         */
        case logout:
        {
            if (tokens.size() != 1)
            {
                cout << "Apenas introduza o comando 'logout'.\n";
                break;
            }

            // Establish UDP connection
            msg = "LOU" + ' ' + curr_user + ' ' + curr_pass + '\n';
            response = connect_UDP(ip_address, port, msg);

            status = splitString(response)[1];

            // Possible status outcomes
            if (status == "UNR")
            {
                cout << "Não pode fazer logout de uma conta não registada.\n";
                logged_in = true;
                break;
            }
            else if (status == "NOK")
            {
                cout << "Não pode fazer logout de uma conta que não se encontra ativa.\n";
                break;
            }
            else if (status == "WRP")
            {
                cout << "Password incorreta.\n";
                break;
            }

            cout << "Logout efetuado com sucesso.\n";
            curr_user = "";
            curr_pass = "";

            logged_in = false;
            break;
        }

        /**
         * Command: exit
         */
        case exit_user:
        {
            if (logged_in)
                cout << "Please log out of your account before exiting.";
            else
                exit_program = true;
            break;
        }

        /**
         * Create command.
         * OK - event created successfully
         * NOK - event could not be created
         * NLG - user was not logged in
         * WRP - incorrect password
         */
        case create:
        {
            /* code */
            break;
        }

        /**
         * Close command.
         * OK - event closed successfully
         * NOK - user does not exist or password is incorrect
         * NLG - user was not logged in
         * NOE - event does not exist
         * EOW -  event was not created by current user
         * SLD - event is sold out
         * PST - event has already happened
         * CLO - event has already been closed
         */
        case close_event:
        {
            if (tokens.size() != 2)
            {
                cout << "Por favor indique o evento que pretende fechar.\n";
                break;
            }

            // Establish TCP connection
            msg = "CLS" + ' ' + curr_user + ' ' + curr_pass + ' ' + tokens[1] + '\n';
            response = connect_TCP(ip_address, port, msg);

            status = splitString(response)[1];

            // Possible status outcomes
            if (status == "NOK")
            {
                cout << "Utilizador não existe ou a password está incorreta.\n";
                logged_in = true;
                break;
            }
            else if (status == "NLG")
            {
                cout << "Por favor faça login primeiro.\n";
                break;
            }
            else if (status == "NOE")
            {
                cout << "Evento não existe.\n";
                break;
            }
            else if (status == "EOW")
            {
                cout << "Evento não foi criado pelo utilizador.\n";
                break;
            }
            else if (status == "SLD")
            {
                cout << "Evento esgotado.\n";
                break;
            }
            else if (status == "PST")
            {
                cout << "Evento já aconteceu.\n";
                break;
            }
            else if (status == "CLO")
            {
                cout << "Evento já se encontrava fechado.\n";
                break;
            }

            cout << "Evento fechado com sucesso.\n";

            break;
        }

        /**
         * myevents command.
         * OK - event created successfully
         * NOK - user has not created any events
         * NLG - user was not logged in
         * WRP - incorrect password
         */
        case myevents:
        {
            if (tokens.size() != 1)
            {
                cout << "Apenas introduza o comando 'myevents' ou 'mye'.\n";
                break;
            }

            // Establish UDP connection
            msg = "LME" + ' ' + curr_user + ' ' + curr_pass + '\n';
            response = connect_UDP(ip_address, port, msg);

            status = splitString(response)[1];

            // Possible status outcomes
            if (status == "NLG")
            {
                cout << "Por favor faça login primeiro.\n";
                logged_in = true;
                break;
            }
            else if (status == "WRP")
            {
                cout << "Password incorreta.\n";
                break;
            }

            cout << response + "\n"; // Manter assim por enquanto, depois separar eventos melhor

            break;
        }

        /**
         * list command.
         * OK - events listed successfully
         * NOK - no events have been created yet
         */
        case list:
        {
            /* code */
            break;
        }

        /**
         * show command.
         * OK - event shown successfully
         * NOK - event does not exist, file could not be sent, others
         */
        case show:
        {
            /* code */
            break;
        }

        /**
         * reserve command.
         * ACC - reservation done successfully
         * NOK - event is not active
         * NLG - user was not logged in
         * WRP - incorrect password
         * REJ - number of tickets is higher than available seats
         * CLS - event is closed
         * SLD - event was sold out
         * PST - event has already happened
         */
        case reserve:
        {
            if (tokens.size() != 3)
            {
                cout << "Por favor indique o evento e o número de lugares que pretende reservar.\n";
                break;
            }

            // Establish TCP connection
            msg = "RID" + ' ' + curr_user + ' ' + curr_pass + ' ' + tokens[1] + ' ' + tokens[2] + '\n';
            response = connect_TCP(ip_address, port, msg);

            status = splitString(response)[1];

            // Possible status outcomes
            if (status == "NOK")
            {
                cout << "Utilizador não existe ou a password está incorreta.\n";
                logged_in = true;
                break;
            }
            else if (status == "NLG")
            {
                cout << "Por favor faça login primeiro.\n";
                break;
            }
            else if (status == "NOE")
            {
                cout << "Evento não existe.\n";
                break;
            }
            else if (status == "EOW")
            {
                cout << "Evento não foi criado pelo utilizador.\n";
                break;
            }
            else if (status == "SLD")
            {
                cout << "Evento esgotado.\n";
                break;
            }
            else if (status == "PST")
            {
                cout << "Evento já aconteceu.\n";
                break;
            }
            else if (status == "CLO")
            {
                cout << "Evento já se encontrava fechado.\n";
                break;
            }

            cout << "Evento fechado com sucesso.\n";

            break;
            break;
        }

        /**
         * myreservations command.
         * OK - reservations shown successfully
         * NOK - user has not made any reservations
         * NLG - user was not logged in
         * WRP - incorrect password
         */
        case myreservations:
        {
            if (tokens.size() != 1)
            {
                cout << "Apenas introduza o comando 'myreservations'.\n";
                break;
            }

            // Establish UDP connection
            msg = "LME" + ' ' + curr_user + ' ' + curr_pass + '\n';
            response = connect_UDP(ip_address, port, msg);

            status = splitString(response)[1];

            // Possible status outcomes
            if (status == "NLG")
            {
                cout << "Por favor faça login primeiro.\n";
                logged_in = true;
                break;
            }
            else if (status == "WRP")
            {
                cout << "Password incorreta.\n";
                break;
            }
            else if (status == "NOK")
            {
                cout << "Ainda não efetou nenhuma reserva.\n";
                break;
            }

            cout << response + "\n"; // Manter assim por enquanto, depois separar eventos melhor

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