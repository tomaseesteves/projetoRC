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

#include <network.hpp>
#include <constants.hpp>
#include <protocol.hpp>
#include <parser.hpp>

using namespace std;

char port[MAX_STRING], ip_address[MAX_STRING];
bool logged_in = false;
string curr_user = "", curr_pass = "";

void handle_ip_port(int argc, char **argv)
{
    // Use default values for IP address and port
    if (argc == 1)
    {
        strcpy(ip_address, IP);
        strcpy(port, PORT);
    }
    // Use IP and port values given by user
    else if (argc == 2)
    {
        strcpy(ip_address, argv[1]);
        strcpy(port, argv[2]);
    }
    else
    {
        cout << "Incorrect number of arguments. Usage: ./user [-n ESIP] [-p ESport]\n";
        exit(1);
    }

    cout << "Bem vindo à nossa plataforma de reserva de eventos!\n";
}

/**
 * Command: login UID password
 * Client message: LIN UID passwors
 * UDP server message: RLI status
 *
 * OK - successful login
 * REG - register new user
 * NOK - password doesn't match UID
 */
void handle_login(vector<string> tokens)
{
    string response, msg, status;
    if (tokens.size() != 3)
    {
        cout << "Por favor introduza o seu username e password.\n";
        return;
    }

    // Establish UDP connection
    msg = "LIN " + tokens[1] + " " + tokens[2] + "\n";
    response = connect_UDP(ip_address, port, msg);

    status = splitString(response)[1];

    // Possible status outcomes
    if (status == "REG")
    {
        cout << "Conta criada com sucesso.\n";
        logged_in = true;
        return;
    }
    else if (status == "NOK")
    {
        cout << "Username ou password incorretos.\n";
        return;
    }
    cout << "Login efetuado com sucesso.\n";
    curr_user = tokens[1];
    curr_pass = tokens[2];
    logged_in = true;
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
void handle_changePass(vector<string> tokens)
{
    string response, msg, status;
    if (tokens.size() != 3)
    {
        cout << "Por favor introduza a password antiga e a password nova.\n";
        return;
    }
    // Establish TCP connection
    msg = "CPS " + curr_user + tokens[1] + " " + tokens[2] + '\n';
    response = connect_TCP(ip_address, port, msg);

    status = splitString(response)[1];

    // Possible status outcomes
    if (status == "NLG")
    {
        cout << "Por favor faça login primeiro.\n";
        return;
    }
    else if (status == "NOK")
    {
        cout << "Password dada não corresponde à password atual.\n";
        return;
    }
    else if (status == "NID")
    {
        cout << "Username fornecido não existe.\n";
        return;
    }

    cout << "Password alterada com sucesso.\n";
    curr_pass = tokens[2];
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
void handle_unregister(vector<string> tokens)
{
    string response, msg, status;
    if (tokens.size() != 1)
    {
        cout << "Apenas introduza o comando 'unregister'.\n";
        return;
    }
    // Establish UDP connection

    msg = "UNR " + curr_user + " " + curr_pass + '\n';
    response = connect_UDP(ip_address, port, msg);

    status = splitString(response)[1];

    // Possible status outcomes
    if (status == "UNR")
    {
        cout << "Não pode remover o registo de uma conta não registada.\n";
        return;
    }
    else if (status == "NOK")
    {
        cout << "Não pode remover o registo de uma conta que não se encontra ativa.\n";
        return;
    }
    else if (status == "WRP")
    {
        cout << "Password incorreta.\n";
        return;
    }

    // REMOVER PASTA DO USER!!!!!
    cout << "Unregister efetuado com sucesso.\n";
    curr_user = "";
    curr_pass = "";
    logged_in = false;
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
void handle_logout(vector<string> tokens)
{
    string response, msg, status;

    if (tokens.size() != 1)
    {
        cout << "Apenas introduza o comando 'logout'.\n";
        return;
    }

    // Establish UDP connection
    msg = "LOU " + curr_user + " " + curr_pass + '\n';
    response = connect_UDP(ip_address, port, msg);

    status = splitString(response)[1];

    // Possible status outcomes
    if (status == "UNR")
    {
        cout << "Não pode fazer logout de uma conta não registada.\n";
        return;
    }
    else if (status == "NOK")
    {
        cout << "Não pode fazer logout de uma conta que não se encontra ativa.\n";
        return;
    }
    else if (status == "WRP")
    {
        cout << "Password incorreta.\n";
        return;
    }

    cout << "Logout efetuado com sucesso.\n";
    curr_user = "";
    curr_pass = "";

    logged_in = false;
}

/**
 * Command: exit
 */
bool handle_exit_user(vector<string> tokens)
{
    if (logged_in)
    {
        cout << "Please log out of your account before exiting.";
        return true;
    }
    return false;
}

/**
 * Create command.
 * OK - event created successfully
 * NOK - event could not be created
 * NLG - user was not logged in
 * WRP - incorrect password
 */
void handle_create(vector<string> tokens)
{
    return;
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
void handle_close_event(vector<string> tokens)
{
    string response, msg, status;
    if (tokens.size() != 2)
    {
        cout << "Por favor indique o evento que pretende fechar.\n";
        return;
    }

    // Establish TCP connection
    msg = "CLS " + curr_user + " " + curr_pass + " " + tokens[1] + '\n';
    response = connect_TCP(ip_address, port, msg);

    status = splitString(response)[1];

    // Possible status outcomes
    if (status == "NOK")
    {
        cout << "Utilizador não existe ou a password está incorreta.\n";
        return;
    }
    else if (status == "NLG")
    {
        cout << "Por favor faça login primeiro.\n";
        return;
    }
    else if (status == "NOE")
    {
        cout << "Evento não existe.\n";
        return;
    }
    else if (status == "EOW")
    {
        cout << "Evento não foi criado pelo utilizador.\n";
        return;
    }
    else if (status == "SLD")
    {
        cout << "Evento esgotado.\n";
        return;
    }
    else if (status == "PST")
    {
        cout << "Evento já aconteceu.\n";
        return;
    }
    else if (status == "CLO")
    {
        cout << "Evento já se encontrava fechado.\n";
        return;
    }

    cout << "Evento fechado com sucesso.\n";
}

/**
 * myevents command.
 * OK - event created successfully
 * NOK - user has not created any events
 * NLG - user was not logged in
 * WRP - incorrect password
 */
void handle_myevents(vector<string> tokens)
{
    string response, msg, status;
    if (tokens.size() != 1)
    {
        cout << "Apenas introduza o comando 'myevents' ou 'mye'.\n";
        return;
    }

    // Establish UDP connection
    msg = "LME " + curr_user + " " + curr_pass + '\n';
    response = connect_UDP(ip_address, port, msg);

    status = splitString(response)[1];

    // Possible status outcomes
    if (status == "NLG")
    {
        cout << "Por favor faça login primeiro.\n";
        logged_in = true;
        return;
    }
    else if (status == "WRP")
    {
        cout << "Password incorreta.\n";
        return;
    }

    cout << response + "\n"; // Manter assim por enquanto, depois separar eventos melhor
}

/**
 * list command.
 * OK - events listed successfully
 * NOK - no events have been created yet
 */
void handle_list(vector<string> tokens)
{
    return;
}

/**
 * show command.
 * OK - event shown successfully
 * NOK - event does not exist, file could not be sent, others
 */
void handle_show(vector<string> tokens)
{
    return;
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
void handle_reserve(vector<string> tokens)
{
    string response, msg, status;
    if (tokens.size() != 3)
    {
        cout << "Por favor indique o evento e o número de lugares que pretende reservar.\n";
        return;
    }

    // Establish TCP connection
    msg = "RID " + curr_user + " " + curr_pass + " " + tokens[1] + " " + tokens[2] + '\n';
    response = connect_TCP(ip_address, port, msg);

    status = splitString(response)[1];

    // Possible status outcomes
    if (status == "NOK")
    {
        cout << "Utilizador não existe ou a password está incorreta.\n";
        return;
    }
    else if (status == "NLG")
    {
        cout << "Por favor faça login primeiro.\n";
        return;
    }
    else if (status == "NOE")
    {
        cout << "Evento não existe.\n";
        return;
    }
    else if (status == "EOW")
    {
        cout << "Evento não foi criado pelo utilizador.\n";
        return;
    }
    else if (status == "SLD")
    {
        cout << "Evento esgotado.\n";
        return;
    }
    else if (status == "PST")
    {
        cout << "Evento já aconteceu.\n";
        return;
    }
    else if (status == "CLO")
    {
        cout << "Evento já se encontrava fechado.\n";
        return;
    }

    cout << "Evento fechado com sucesso.\n";
}

/**
 * myreservations command.
 * OK - reservations shown successfully
 * NOK - user has not made any reservations
 * NLG - user was not logged in
 * WRP - incorrect password
 */
void handle_myreservations(vector<string> tokens)
{
    string response, msg, status;
    if (tokens.size() != 1)
    {
        cout << "Apenas introduza o comando 'myreservations'.\n";
        return;
    }

    // Establish UDP connection
    msg = "LME " + curr_user + " " + curr_pass + '\n';
    response = connect_UDP(ip_address, port, msg);

    status = splitString(response)[1];

    // Possible status outcomes
    if (status == "NLG")
    {
        cout << "Por favor faça login primeiro.\n";
        return;
    }
    else if (status == "WRP")
    {
        cout << "Password incorreta.\n";
        return;
    }
    else if (status == "NOK")
    {
        cout << "Ainda não efetou nenhuma reserva.\n";
        return;
    }

    cout << response + "\n"; // Manter assim por enquanto, depois separar eventos melhor
}