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
#include <file.hpp>

using namespace std;

char port[MAX_STRING], ip_address[MAX_STRING];
bool logged_in = false;
string curr_user = "", curr_pass = "";

void handle_ip_port(int argc, char **argv)
{
    // Use default values for IP address and port
    if (argc == 1)
    { // mudar depois!!!!!
        strcpy(ip_address, "193.136.138.142");
        strcpy(port, "58000");
    }
    // Use IP and port values given by user
    else if (argc == 3)
    {
        strcpy(ip_address, argv[1]);
        strcpy(port, argv[2]);
    }
    else
    {
        cout << "Incorrect number of arguments. Usage: ./user [-n ESIP] [-p ESport]\n\n";
        exit(1);
    }

    cout << "Welcome!\n\n";
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
    string response, status, msg, reply_command;
    vector<string> divided_response;

    if (tokens.size() != 3)
    {
        cout << "Incorrect number of arguments.\nUsage: login [UID] [password]\n\n";
        return;
    }
    else if (!check_size_uid(tokens[1]))
    {
        cout << "UserID must have exactly 6 digits.\n\n";
        return;
    }
    else if (!check_size_password(tokens[2]))
    {
        cout << "Password must have exactly 8 characters.\n\n";
        return;
    }
    else if (!check_only_digits(tokens[1]))
    {
        cout << "UserID must have only digits.\n\n";
        return;
    }
    else if (!check_only_alphanumerical(tokens[2]))
    {
        cout << "Password must have only alphanumeric characters.\n";
        return;
    }

    // Establish UDP connection
    msg = "LIN " + tokens[1] + " " + tokens[2] + '\n';
    response = connect_UDP(ip_address, port, msg);

    divided_response = splitString(response);
    reply_command = trim(divided_response[0]);
    status = trim(divided_response[1]);

    if (reply_command == "ERR")
    {
        cout << "Unexpected protocol message was received.\n\n";
        return;
    }

    // Possible status outcomes
    if (status == "OK")
    {
        cout << "Logged in successfully. Welcome " + tokens[1] + "!\n\n";
        curr_user = tokens[1];
        curr_pass = tokens[2];
        logged_in = true;
        return;
    }
    else if (status == "REG")
    {
        cout << "New account created successfully. Welcome " + tokens[1] + "!\n\n";
        curr_user = tokens[1];
        curr_pass = tokens[2];
        logged_in = true;
        return;
    }
    else if (status == "NOK")
    {
        cout << "Failed to authenticate user. UserID or password are incorrect.\n\n";
        return;
    }
    else if (status == "ERR")
    {
        cout << "Syntax of the request message is incorrect.\n\n";
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
        cout << "Incorrect number of arguments.\nUsage: changePass [oldPassword] [newPassword]\n\n";
        return;
    }
    else if (!check_size_password(tokens[1]))
    {
        cout << "Old password must have exactly 8 characters.\n\n";
        return;
    }
    else if (!check_size_password(tokens[2]))
    {
        cout << "New password must have exactly 8 characters.\n\n";
        return;
    }
    else if (!check_only_alphanumerical(tokens[1]))
    {
        cout << "Old password must have only alphanumeric characters.\n\n";
        return;
    }
    else if (!check_only_alphanumerical(tokens[2]))
    {
        cout << "New password must have only alphanumeric characters.\n\n";
        return;
    }

    // Establish TCP connection
    msg = "CPS " + curr_user + tokens[1] + " " + tokens[2] + '\n';
    response = connect_TCP(ip_address, port, msg);

    divided_response = splitString(response);
    reply_command = trim(divided_response[0]);
    status = trim(divided_response[1]);

    if (reply_command == "ERR")
    {
        cout << "Unexpected protocol message was received.\n\n";
        return;
    }

    // Possible status outcomes
    if (status == "OK")
    {
        cout << "Password changed successfuly.\n\n";
        curr_pass = tokens[2];
        return;
    }
    else if (status == "NLG")
    {
        cout << "Please login into an account before changing the password.\n\n";
        return;
    }
    else if (status == "NOK")
    {
        cout << "The old password provided does not correspond to the current password.\n\n";
        return;
    }
    else if (status == "NID")
    {
        cout << "Provided UserID does not exist.\n\n";
        return;
    }
    else if (status == "ERR")
    {
        cout << "Syntax of the request message is incorrect.\n\n";
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
        cout << "Incorrect number of arguments.\nUsage: unregister\n\n";
        return;
    }
    // Establish UDP connection

    msg = "UNR " + curr_user + " " + curr_pass + '\n';
    response = connect_UDP(ip_address, port, msg);

    divided_response = splitString(response);
    reply_command = trim(divided_response[0]);
    status = trim(divided_response[1]);

    if (reply_command == "ERR")
    {
        cout << "Unexpected protocol message was received.\n\n";
        return;
    }

    // Possible status outcomes
    if (status == "OK")
    {
        // REMOVER PASTA DO USER????
        cout << "Unregistered successfuly.\n\n";
        curr_user = "";
        curr_pass = "";
        logged_in = false;
    }
    if (status == "UNR")
    {
        cout << "The account under UserID does not exist.\n\n";
        return;
    }
    else if (status == "NOK")
    {
        cout << "Please login into an account before unregistering.\n\n";
        return;
    }
    else if (status == "WRP")
    {
        cout << "Wrong password.\n\n";
        return;
    }
    else if (status == "ERR")
    {
        cout << "Syntax of the request message is incorrect\n\n";
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
        cout << "Incorrect number of arguments.\nUsage: logout\n\n";
        return;
    }

    // Establish UDP connection
    msg = "LOU " + curr_user + " " + curr_pass + '\n';
    response = connect_UDP(ip_address, port, msg);

    divided_response = splitString(response);
    reply_command = trim(divided_response[0]);
    status = trim(divided_response[1]);

    if (reply_command == "ERR")
    {
        cout << "Unexpected protocol message was received.\n\n";
        return;
    }

    // Possible status outcomes
    if (status == "OK")
    {
        cout << "Logged out successfully.\n\n";
        curr_user = "";
        curr_pass = "";

        logged_in = false;
    }
    else if (status == "UNR")
    {
        cout << "Cannot logout of a unregistered account.\n\n";
        return;
    }
    else if (status == "NOK")
    {
        cout << "Please login into an account before logging out.\n\n";
        return;
    }
    else if (status == "WRP")
    {
        cout << "Incorrect password.\n\n";
        return;
    }
    else if (status == "ERR")
    {
        cout << "Syntax of the request message is incorrect.\n\n";
        return;
    }
    return;
}

/**
 * Command: exit
 */
bool handle_exit_user(vector<string> tokens)
{
    if (tokens.size() != 1)
    {
        cout << "Incorrect number of arguments.\nUsage: exit\n\n";
        return false;
    }
    if (logged_in)
    {
        cout << "Please log out of your account before exiting.\n\n";
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
    int file_size;
    string file_content, response, msg, status, reply_command;
    vector<string> divided_response;

    if (tokens.size() != 6)
    {
        cout << "Incorrect number of arguments.\nUsage: create [name] [event_fname] [event_date] [num_attendees]\n\n";
        return;
    }
    else if (!check_size_event_name(tokens[1]))
    {
        cout << "Name of event must have exactly 10 characters.\n\n";
        return;
    }
    else if (!check_only_alphanumerical(tokens[1]))
    {
        cout << "Name of event must have only alphanumeric characters.\n\n";
        return;
    }
    else if (!check_only_digits(tokens[5]) || stoi(tokens[5]) < 10 || stoi(tokens[5]) > 99)
    {
        cout << "Number of seats in event must be a number between 10 and 999.\n\n";
        return;
    }
    else if (!check_size_file_name(tokens[2]) || !check_file_name(tokens[2]))
    {
        cout << "Name of event file is not valid.\n\n";
        return;
    }

    if (!extract_file(tokens[2], file_content, file_size))
    {
        cout << "Could not open or read the file's content.\n\n";
        return;
    }

    if (!check_size_file(file_size))
    {
        cout << "File is too large to be sent.\n\n";
        return;
    }

    // Establish TCP connection
    msg = "CRE " + curr_user + " " + curr_pass + " " +
          tokens[1] + " " + tokens[3] + " " + tokens[4] + " " + tokens[5] + " " +
          tokens[2] + " " + to_string(file_size) + " " + file_content + '\n';
    response = connect_TCP(ip_address, port, msg);

    divided_response = splitString(response);
    reply_command = trim(divided_response[0]);
    status = trim(divided_response[1]);

    if (reply_command == "ERR")
    {
        cout << "Unexpected protocol message was received.\n\n";
        return;
    }

    // Possible status outcomes
    if (status == "OK")
    {
        cout << "Event created successfully! The event's ID is " + trim(divided_response[2]) + ".\n\n";
        return;
    }
    else if (status == "NOK")
    {
        cout << "Event could not be created.\n\n";
        return;
    }
    else if (status == "NLG")
    {
        cout << "Please login into an account before creating an event.\n\n";
        return;
    }
    else if (status == "ERR")
    {
        cout << "Syntax of the request message is incorrect.\n\n";
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
 * CLS - event has already been closed
 */
void handle_close_event(vector<string> tokens)
{
    string response, msg, status, reply_command;
    vector<string> divided_response;
    if (tokens.size() != 2)
    {
        cout << "Incorrect number of arguments.\nUsage: close [EventID]\n\n";
        return;
    }
    else if (!check_size_eid(tokens[1]))
    {
        cout << "EventID must have exactly 3 digits.\n\n";
        return;
    }
    else if (!check_only_digits(tokens[1]))
    {
        cout << "EventID must have only digits.\n\n";
        return;
    }

    // Establish TCP connection
    msg = "CLS " + curr_user + " " + curr_pass + " " + tokens[1] + '\n';
    response = connect_TCP(ip_address, port, msg);

    divided_response = splitString(response);
    reply_command = trim(divided_response[0]);
    status = trim(divided_response[1]);

    if (reply_command == "ERR")
    {
        cout << "Unexpected protocol message was received.\n\n";
        return;
    }
    // Possible status outcomes
    if (status == "OK")
    {
        cout << "Event closed successfully.\n\n";
        return;
    }
    if (status == "NOK")
    {
        cout << "Failed to authenticate user. UserID or password are incorrect.\n\n";
        return;
    }
    else if (status == "NLG")
    {
        cout << "Please login into an account before closing an event.\n\n";
        return;
    }
    else if (status == "NOE")
    {
        cout << "Given event does not exist.\n\n";
        return;
    }
    else if (status == "EOW")
    {
        cout << "You're not allowed to close events created by other accounts.\n\n";
        return;
    }
    else if (status == "SLD")
    {
        cout << "Event is currently sold out.\n\n";
        return;
    }
    else if (status == "PST")
    {
        cout << "Event has already happened.\n\n";
        return;
    }
    else if (status == "CLS")
    {
        cout << "Event had already been closed before.\n\n";
        return;
    }
    else if (status == "ERR")
    {
        cout << "Syntax of the request message is incorrect.\n\n";
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
        cout << "Incorrect number of arguments.\nUsage: myevents OR mye\n\n";
        return;
    }

    // Establish UDP connection
    msg = "LME " + curr_user + " " + curr_pass + '\n';
    response = connect_UDP(ip_address, port, msg);

    divided_response = splitString(response);
    reply_command = trim(divided_response[0]);
    status = trim(divided_response[1]);

    if (reply_command == "ERR")
    {
        cout << "Unexpected protocol message was received.\n\n";
        return;
    }

    // Possible status outcomes
    if (status == "OK")
    {
        response_size = divided_response.size();
        cout << "Your events:\n";
        for (int i = 2; (size_t)i < response_size; i += 2)
        {
            cout << "-> Event " + divided_response[i] + ": " + resolveState(divided_response[i + 1]) + ".\n";
        }
        cout << "Here are all of your events!\n\n";
        return;
    }
    if (status == "NLG")
    {
        cout << "Please login into your account before checking your created events.\n\n";
        logged_in = true;
        return;
    }
    else if (status == "NOK")
    {
        cout << "No events have been created yet.\n\n";
        return;
    }
    else if (status == "WRP")
    {
        cout << "Incorrect password.\n\n";
        return;
    }
    else if (status == "ERR")
    {
        cout << "Syntax of the request message is incorrect.\n\n";
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
        cout << "Incorrect number of arguments.\nUsage: list\n\n";
        return;
    }

    // Establish TCP connection
    msg = "LST\n";
    response = connect_TCP(ip_address, port, msg);

    divided_response = splitString(response);
    reply_command = trim(divided_response[0]);
    status = trim(divided_response[1]);

    if (reply_command == "ERR")
    {
        cout << "Unexpected protocol message was received.\n\n";
        return;
    }

    // Possible status outcomes
    if (status == "OK")
    {
        cout << "Current Events:\n";
        response_size = divided_response.size();
        for (int i = 2; (size_t)i < response_size; i += 5)
        {
            cout << "-> Event " + divided_response[i + 1] + " (ID: " + divided_response[i] + "): " + resolveState(divided_response[i + 2]) +
                        "\n   (Event's date: " + divided_response[i + 3] + " " + trim(divided_response[i + 4]) + ")\n";
        }
        cout << "Full Events List!\n\n";
        return;
    }
    else if (status == "NOK")
    {
        cout << "No events have been created yet.\n\n";
        return;
    }
    else if (status == "ERR")
    {
        cout << "Syntax of the request message is incorrect.\n\n";
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
    string response, msg, status, reply_command, file_data, file_content;
    vector<string> divided_response, divided_data;
    uint64_t file_size;

    if (tokens.size() != 2)
    {
        cout << "Incorrect number of arguments.\nUsage: show [EventID]\n\n";
        return;
    }
    else if (!check_size_eid(tokens[1]))
    {
        cout << "EventID must have exactly 3 digits.\n\n";
        return;
    }
    else if (!check_only_digits(tokens[1]))
    {
        cout << "EventID must have only digits.\n\n";
        return;
    }

    // Establish TCP connection
    msg = "SED " + tokens[1] + '\n';
    response = connect_TCP(ip_address, port, msg);

    if (response.find(" OK") != string::npos)
    {
        divided_response = extract_file_data(response, 0);
        file_data = divided_response[1];
        divided_data = splitString(file_data);
        file_size = stoull(divided_data[7]);

        file_content = trim(divided_response[2]);

        if (save_event_file(divided_data[6], file_content, file_size))
        {
            cout << "Received event " + divided_data[1] + " created by user " + divided_data[0] + ":\n"
                                                                                                  "-> Event's date " +
                        divided_data[2] + " " + divided_data[3] + "\n-> Out of " +
                        divided_data[4] + " seats, " + divided_data[5] + " seats have been reserved.\n-> File " + divided_data[6] + " of size " + divided_data[7] + " Bytes has been saved to your local directory.\n\n";
        }
        return;
    }

    divided_response = splitString(response);
    reply_command = trim(divided_response[0]);
    status = trim(divided_response[1]);

    if (reply_command == "ERR")
    {
        cout << "Unexpected protocol message was received.\n\n";
        return;
    }

    // Possible status outcomes
    else if (status == "NOK")
    {
        cout << "Unexpected error, please try again.\n\n";
        return;
    }
    else if (status == "ERR")
    {
        cout << "Syntax of the request message is incorrect.\n\n";
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
        cout << "Incorrect number of arguments.\nUsage: reserve [EventID] [value]\n\n";
        return;
    }
    else if (!check_size_eid(tokens[1]))
    {
        cout << "EventID must have exactly 3 digits.\n\n";
        return;
    }
    else if (!check_only_digits(tokens[1]))
    {
        cout << "EventID must have only digits.\n\n";
        return;
    }
    else if (!check_only_digits(tokens[2]) || stoi(tokens[2]) < 1 || stoi(tokens[2]) > 999)
    {
        cout << "Number of seats in event must be a number between 10 and 999.\n\n";
        return;
    }

    // Establish TCP connection
    msg = "RID " + curr_user + " " + curr_pass + " " + tokens[1] + " " + tokens[2] + '\n';
    response = connect_TCP(ip_address, port, msg);

    divided_response = splitString(response);
    reply_command = trim(divided_response[0]);
    status = trim(divided_response[1]);

    if (reply_command == "ERR")
    {
        cout << "Unexpected protocol message was received.\n\n";
        return;
    }

    // Possible status outcomes
    if (status == "ACC")
    {
        cout << "Reservation made successfully. Enjoy the event!\n\n";
        return;
    }
    else if (status == "NOK")
    {
        cout << "Event is not currently active.\n\n";
        return;
    }
    else if (status == "NLG")
    {
        cout << "Please login into an account before making a reservation.\n\n";
        return;
    }
    else if (status == "SLD")
    {
        cout << "Event was sold out.\n\n";
        return;
    }
    else if (status == "PST")
    {
        cout << "Event has already happened.\n\n";
        return;
    }
    else if (status == "CLS")
    {
        cout << "Event has already been closed.\n\n";
        return;
    }
    else if (status == "REJ")
    {
        remaining_seats = trim(divided_response[2]);
        cout << "The reservation was rejected.\n The number of requested places, " +
                    tokens[2] + ", is larger than the number of remaining places, which is " +
                    remaining_seats + ".\n\n";
        return;
    }
    else if (status == "WRP")
    {
        cout << "Incorrect password.\n\n";
        return;
    }
    else if (status == "ERR")
    {
        cout << "Syntax of the request message is incorrect.\n\n";
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
        cout << "Incorrect number of arguments.\nUsage: myreservations OR myr\n\n";
        return;
    }

    // Establish UDP connection
    msg = "LMR " + curr_user + " " + curr_pass + '\n';
    response = connect_UDP(ip_address, port, msg);

    divided_response = splitString(response);
    reply_command = trim(divided_response[0]);
    status = trim(divided_response[1]);

    if (reply_command == "ERR")
    {
        cout << "Unexpected protocol message was received.\n\n";
        return;
    }

    // Possible status outcomes
    if (status == "OK")
    {
        response_size = divided_response.size();
        cout << "Your reservations:\n";
        for (int i = 2; (size_t)i < response_size; i += 4)
        {
            cout << "-> Reservation for event " + divided_response[i] + ": " + trim(divided_response[i + 3]) + " seats reserved.\n" + "   (Reservation made at date " + divided_response[i + 1] + " " + divided_response[i + 2] + ")\n";
        }
        cout << "Remember, only the most recent 50 reservations are shown!\n\n";
        return;
    }
    if (status == "NLG")
    {
        cout << "Please login into your account before checking your reservations.\n\n";
        return;
    }
    else if (status == "WRP")
    {
        cout << "Incorrect password.\n\n";
        return;
    }
    else if (status == "NOK")
    {
        cout << "No reservations have been made yet.\n\n";
        return;
    }
    else if (status == "ERR")
    {
        cout << "Syntax of the request message is incorrect.\n\n";
        return;
    }
    return;
}