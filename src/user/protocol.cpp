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
#include <utils.hpp>

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

    cout << "Welcome!\n";
}

/// o user tem de verificar os replies do server?
/// gostaria de mudar as mensagens para inglês
///  (O QUE SAO LETRAS UNCLUI SO ASCII)??
/// nao se encontra ativa parece que a conta foi desativada
/// lembrar de verificafr o primeira parte da mensagem de status devido a receber ERR
/// seria boa ideia meter id do user na mensagem stdout
/// status tambem podem ter ERR
/*
The filenames Fname, are limited to a total of 24 alphanumerical characters (plus ‘-‘, ‘_’
and ‘.’), including the separating dot and the 3-letter extension: “nnn…nnnn.xxx”.
The file size Fsize is limited to 10 MB (10.106 B), being transmitted using a maximum
of 8 digits*/
/// create new event requer nome (alphanumerical numbers), file , date representado por dd-mm-yyyy hh:mm e number available seats (10 < seats < 999)
/// reserve n tem de ser menor ou igual ao available seats
/// ver quem indentifica os erros USER OU SERVER
/// server ve a lista de as reservas mais recentes 50 max
/// o max string é o numero de bytes maximo que se pode recer?

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
    string response, msg, status, reply_command;
    vector<string> divided_response;
    if (tokens.size() != 3)
    {
        cout << "Por favor introduza o seu username e password.\n";
        return;
    }
    else if (!check_size_uid(tokens[1]))
    {
        cout << "UserID só deve conter 6 digitos\n";
        return;
    }
    else if (!check_size_password(tokens[2]))
    {
        cout << "Palavra passe só deve conter 8 dígitos\n";
        return;
    }
    else if (!check_only_digits(tokens[1]))
    {
        cout << "User<id só deve conter digítos\n";
        return;
    }
    else if (!check_only_alphanumerical(tokens[2]))
    {
        cout << "A palavra passe só deve conter digítos ou letras\n";
        return;
    }

    // Establish UDP connection
    msg = "LIN " + tokens[1] + " " + tokens[2] + "\n";
    response = connect_UDP(ip_address, port, msg);

    divided_response = splitString(response);
    reply_command = divided_response[0];
    status = splitString(response)[1];

    if (reply_command == "ERR")
    {
        cout << "Unexpected protocol message was received\n";
        return;
    }

    // Possible status outcomes
    if (status == "OK")
    {
        cout << "Login efetuado com sucesso.\n";
        curr_user = tokens[1];
        curr_pass = tokens[2];
        logged_in = true;
    }
    if (status == "REG")
    {
        cout << "Conta criada com sucesso.\n";
        curr_user = tokens[1];
        curr_pass = tokens[2];
        logged_in = true;
        return;
    }
    else if (status == "NOK")
    {
        cout << "Username ou password incorretos.\n";
        return;
    }
    else if (status == "ERR")
    {
        cout << "Syntax of the request message is incorrect\n";
        return;
    }
    return;
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
    string response, msg, status, reply_command;
    vector<string> divided_response;
    if (tokens.size() != 3)
    {
        cout << "Por favor introduza a password antiga e a password nova.\n";
        return;
    }
    else if (!check_size_password(tokens[1]))
    {
        cout << "palavra passe antiga só deve conter 8 dígitos\n";
        return;
    }
    else if (!check_size_password(tokens[2]))
    {
        cout << "palavra passe nova só deve conter 8 dígitos\n";
        return;
    }
    else if (!check_only_alphanumerical(tokens[1]))
    {
        cout << "A palavra passe antiga só deve conter digítos ou letras\n";
        return;
    }
    else if (!check_only_alphanumerical(tokens[2]))
    {
        cout << "A palavra passe nova só deve conter digítos ou letras\n";
        return;
    }

    // Establish TCP connection
    msg = "CPS " + curr_user + tokens[1] + " " + tokens[2] + '\n';
    response = connect_TCP(ip_address, port, msg);

    divided_response = splitString(response);
    reply_command = divided_response[0];
    status = splitString(response)[1];

    if (reply_command == "ERR")
    {
        cout << "unexpected protocol message was received\n";
        return;
    }
    // Possible status outcomes
    if (status == "OK")
    {
        cout << "Password alterada com sucesso.\n";
        curr_pass = tokens[2];
        return;
    }
    else if (status == "NLG")
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
    else if (status == "ERR")
    {
        cout << "syntax of the request message is incorrect\n";
        return;
    }
    return;
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
    string response, msg, status, reply_command;
    vector<string> divided_response;
    if (tokens.size() != 1)
    {
        cout << "Apenas introduza o comando 'unregister'.\n";
        return;
    }
    // Establish UDP connection

    msg = "UNR " + curr_user + " " + curr_pass + '\n';
    response = connect_UDP(ip_address, port, msg);

    divided_response = splitString(response);
    reply_command = divided_response[0];
    status = splitString(response)[1];

    if (reply_command == "ERR")
    {
        cout << "unexpected protocol message was received\n";
        return;
    }

    // Possible status outcomes
    if (status == "OK")
    {
        // REMOVER PASTA DO USER!!!!!
        cout << "Unregister efetuado com sucesso.\n";
        curr_user = "";
        curr_pass = "";
        logged_in = false;
    }
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
    else if (status == "ERR")
    {
        cout << "syntax of the request message is incorrect\n";
        return;
    }
    return;
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
    string response, msg, status, reply_command;
    vector<string> divided_response;

    if (tokens.size() != 1)
    {
        cout << "Apenas introduza o comando 'logout'.\n";
        return;
    }

    // Establish UDP connection
    msg = "LOU " + curr_user + " " + curr_pass + '\n';
    response = connect_UDP(ip_address, port, msg);

    divided_response = splitString(response);
    reply_command = divided_response[0];
    status = splitString(response)[1];

    if (reply_command == "ERR")
    {
        cout << "unexpected protocol message was received\n";
        return;
    }
    // Possible status outcomes
    if (status == "OK")
    {
        cout << "Logout efetuado com sucesso.\n";
        curr_user = "";
        curr_pass = "";

        logged_in = false;
    }
    else if (status == "UNR")
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
    else if (status == "ERR")
    {
        cout << "syntax of the request message is incorrect\n";
        return;
    }
    return;
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
    size_t response_size;
    string response, msg, status, reply_command;
    vector<string> divided_response;

    if (tokens.size() != 5)
    {
        cout << "Número incorreto de argumentos.\n";
        return;
    }
    else if (!check_size_event_name(tokens[1]))
    {
        cout << "A descrição do evento deve conter até 10 caracteres\n";
        return;
    }
    else if (!check_only_alphanumerical(tokens[1]))
    {
        cout << "A descrição do evento deve só conter alphanumericos.\n";
        return;
    }
    else if (!check_only_digits(tokens[5]) || stoi(tokens[5]) < 10 || stoi(tokens[5]) > 99)
    {
        cout << "O numero de lugares disponíveis deve ser um inteiro entre 10 a 999\n";
        return;
    }
    else if (!check_size_file_name(tokens[2]) || !check_file_name(tokens[2]))
    {
        cout << "O nome do ficheiro está incorreto.\n";
        return;
    }
    // Establish TCP connection
    msg = "CRE " + curr_user + " " + curr_pass + " " +
          tokens[1] + " " + tokens[2] + " " + tokens[3] + "\n";
    response = connect_TCP(ip_address, port, msg);

    divided_response = splitString(response);
    reply_command = divided_response[0];
    status = splitString(response)[1];

    if (reply_command == "ERR")
    {
        cout << "unexpected protocol message was received\n";
        return;
    }
    // Possible status outcomes
    if (status == "OK")
    {
        return;
    }
    else if (status == "NOK")
    {
        cout << "não há eventos criados.\n";
        return;
    }
    else if (status == "ERR")
    {
        cout << "syntax of the request message is incorrect\n";
        return;
    }
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
    string response, msg, status, reply_command;
    vector<string> divided_response;
    if (tokens.size() != 2)
    {
        cout << "Por favor indique o evento que pretende fechar.\n";
        return;
    }
    else if (!check_size_eid(tokens[1]))
    {
        cout << "EID só deve conter 3 digitos\n";
        return;
    }
    else if (!check_only_digits(tokens[1]))
    {
        cout << "EID só deve conter dígitos\n";
        return;
    }

    // Establish TCP connection
    msg = "CLS " + curr_user + " " + curr_pass + " " + tokens[1] + '\n';
    response = connect_TCP(ip_address, port, msg);

    divided_response = splitString(response);
    reply_command = divided_response[0];
    status = splitString(response)[1];

    if (reply_command == "ERR")
    {
        cout << "unexpected protocol message was received\n";
        return;
    }
    // Possible status outcomes
    if (status == "OK")
    {
        cout << "Evento fechado com sucesso.\n";
        return;
    }
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
    else if (status == "ERR")
    {
        cout << "syntax of the request message is incorrect\n";
        return;
    }
    return;
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
    size_t response_size;
    string response, msg, status, reply_command;
    vector<string> divided_response;
    if (tokens.size() != 1)
    {
        cout << "Apenas introduza o comando 'myevents' ou 'mye'.\n";
        return;
    }

    // Establish UDP connection
    msg = "LME " + curr_user + " " + curr_pass + '\n';
    response = connect_UDP(ip_address, port, msg);

    divided_response = splitString(response);
    reply_command = divided_response[0];
    status = splitString(response)[1];

    if (reply_command == "ERR")
    {
        cout << "unexpected protocol message was received\n";
        return;
    }
    // Possible status outcomes
    if (status == "OK")
    {
        response_size = divided_response.size();
        for (int i = 2; i < response_size; i += 2)
        {
            cout << divided_response[i] + divided_response[i + 1] + "\n";
        }
        return;
    }
    if (status == "NLG")
    {
        cout << "Por favor faça login primeiro.\n";
        logged_in = true;
        return;
    }
    else if (status == "NOK")
    {
        cout << "não há eventos criados.\n";
        return;
    }
    else if (status == "WRP")
    {
        cout << "Password incorreta.\n";
        return;
    }
    else if (status == "ERR")
    {
        cout << "syntax of the request message is incorrect\n";
        return;
    }
    return;
}

/**
 * list command.
 * OK - events listed successfully
 * NOK - no events have been created yet
 */
void handle_list(vector<string> tokens)
{
    size_t response_size;
    string response, msg, status, reply_command;
    vector<string> divided_response;
    if (tokens.size() != 1)
    {
        cout << "Apenas introduza o comando 'list'\n";
        return;
    }

    // Establish TCP connection
    msg = "LST\n";
    response = connect_TCP(ip_address, port, msg);

    divided_response = splitString(response);
    reply_command = divided_response[0];
    status = splitString(response)[1];

    if (reply_command == "ERR")
    {
        cout << "unexpected protocol message was received\n";
        return;
    }
    // Possible status outcomes
    if (status == "OK")
    {
        response_size = divided_response.size();
        for (int i = 2; i < response_size; i += 4)
        {
            cout << divided_response[i] + divided_response[i + 1] +
                        divided_response[i + 2] + divided_response[i + 4] + "\n";
        }
        return;
    }
    else if (status == "NOK")
    {
        cout << "não há eventos criados.\n";
        return;
    }
    else if (status == "ERR")
    {
        cout << "syntax of the request message is incorrect\n";
        return;
    }
    return;
}

/**
 * show command.
 * OK - event shown successfully
 * NOK - event does not exist, file could not be sent, others
 */
void handle_show(vector<string> tokens)
{
    size_t response_size;
    string response, msg, status, reply_command;
    vector<string> divided_response;
    if (tokens.size() != 2)
    {
        cout << "Por favor indique o evento a colocar\n";
        return;
    }
    else if (!check_size_eid(tokens[1]))
    {
        cout << "EID só deve conter 3 digitos\n";
        return;
    }
    else if (!check_only_digits(tokens[1]))
    {
        cout << "EID só deve conter dígitos\n";
        return;
    }

    // Establish TCP connection
    msg = "SED " + tokens[2] + '\n';
    response = connect_TCP(ip_address, port, msg);

    divided_response = splitString(response);
    reply_command = divided_response[0];
    status = splitString(response)[1];

    if (reply_command == "ERR")
    {
        cout << "unexpected protocol message was received\n";
        return;
    }
    // Possible status outcomes
    if (status == "OK")
    {
        response_size = divided_response.size();
        /// ficheiro que recebe pela comunicação tcp tem de ser criado no current
        cout << divided_response[7] + divided_response[8] + "\n";

        return;
    }
    else if (status == "NOK")
    {
        cout << "event does not exist, file could not be sent, others.\n";
        return;
    }
    else if (status == "ERR")
    {
        cout << "syntax of the request message is incorrect\n";
        return;
    }
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
    string response, msg, status, reply_command;
    vector<string> divided_response;
    string remaining_seats;
    if (tokens.size() != 3)
    {
        cout << "Por favor indique o evento e o número de lugares que pretende reservar.\n";
        return;
    }
    else if (!check_size_eid(tokens[1]))
    {
        cout << "EID só deve conter 3 digitos\n";
        return;
    }
    else if (!check_only_digits(tokens[1]))
    {
        cout << "EID só deve conter dígitos\n";
        return;
    }
    else if (!check_only_digits(tokens[2]) || stoi(tokens[2]) < 1 || stoi(tokens[2]) > 999)
    {
        cout << "o numero de pessoas devem ser entre 1 a 999\n";
        return;
    }

    // Establish TCP connection
    msg = "RID " + curr_user + " " + curr_pass + " " + tokens[1] + " " + tokens[2] + '\n';
    response = connect_TCP(ip_address, port, msg);

    divided_response = splitString(response);
    reply_command = divided_response[0];
    status = splitString(response)[1];

    if (reply_command == "ERR")
    {
        cout << "unexpected protocol message was received\n";
        return;
    }

    // Possible status outcomes
    if (status == "ACC")
    {
        cout << "reserva pode ser satisfeita\n";
        return;
    }
    if (status == "NOK")
    {
        cout << "evento não está ativo\n";
        return;
    }
    else if (status == "NLG")
    {
        cout << "Por favor faça login primeiro.\n";
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
    else if (status == "REJ")
    {
        remaining_seats = divided_response[2];
        cout << "the reservation was rejected because the number of requested places" +
                    tokens[2] + "is larger than the number of remaining places." +
                    remaining_seats + "\n";
        return;
    }
    else if (status == "WRP")
    {
        cout << "Password incorreta.\n";
        return;
    }
    else if (status == "ERR")
    {
        cout << "syntax of the request message is incorrect\n";
        return;
    }
    return;
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
    size_t response_size;
    string response, msg, status, reply_command;
    vector<string> divided_response;
    if (tokens.size() != 1)
    {
        cout << "Apenas introduza o comando 'myreservations'.\n";
        return;
    }

    // Establish UDP connection
    msg = "LME " + curr_user + " " + curr_pass + '\n';
    response = connect_UDP(ip_address, port, msg);

    divided_response = splitString(response);
    reply_command = divided_response[0];
    status = splitString(response)[1];

    if (reply_command == "ERR")
    {
        cout << "unexpected protocol message was received\n";
        return;
    }

    // Possible status outcomes
    if (status == "OK")
    {
        response_size = divided_response.size();
        for (int i = 2; i < response_size; i += 2)
        {
            cout << divided_response[i] + divided_response[i + 1] + "\n";
        }
        return;
    }
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
    else if (status == "ERR")
    {
        cout << "syntax of the request message is incorrect\n";
        return;
    }
    return;
}