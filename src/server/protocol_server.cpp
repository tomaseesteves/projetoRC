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
#include <parser.hpp>
#include <utils.hpp>
#include <protocol_server.hpp>
#include <check_server_requests.hpp>

using namespace std;

/// VER O PORT E O IPADRESS DO PROTOCOL CLIENTE!!! PERGUNTAR AO PROFESSOR
/// CRIAR UMA MELHOR VERIFICAÇAO PARA AMBOS OS LADOS QUANTO AO QUE ESTA A RECEBER
/// CHECK DATE E CHECK HOUR !!!!
/// filesize tem de ter max 8 digitos
/// verificar no create que a data que o file recebe corresponde ao numero de bytes corretos

string handle_login(vector<string> tokens)
{
    string msg;
    if (tokens.size() != 3 || !check_size_uid(tokens[1]) ||
        !check_size_password(tokens[2]) || !check_only_digits(tokens[1]) ||
        !check_only_alphanumerical(tokens[2]))
    {
        msg = "RLI ERR\n";
        return msg;
    }

    switch (check_login(tokens[1], tokens[2]))
    {
    case OK:
    {
        msg = "RLI OK\n";
        return msg;
    }
    case NOK:
    {
        msg = "RLI NOK\n";
        return msg;
    }
    case REG:
    {
        msg = "RLI REG\n";
        return msg;
    }
    }
    return "ERR\n";
}

string handle_logout(vector<string> tokens)
{
    string msg;
    if (tokens.size() != 3 || !check_size_uid(tokens[1]) ||
        !check_size_password(tokens[2]) || !check_only_digits(tokens[1]) ||
        !check_only_alphanumerical(tokens[2]))
    {
        msg = "RLO ERR\n";
        return msg;
    }
    switch (check_logout(tokens[1], tokens[2]))
    {
    case OK:
    {
        msg = "RLO OK\n";
        return msg;
    }
    case NOK:
    {
        msg = "RLO NOK\n";
        return msg;
    }
    case UNR:
    {
        msg = "RLO UNR\n";
        return msg;
    }
    case WRP:
    {
        msg = "RLO WRP\n";
        return msg;
    }
    }
    return "ERR\n";
}

string handle_unregister(vector<string> tokens)
{
    string msg;
    if (tokens.size() != 3 || !check_size_uid(tokens[1]) ||
        !check_size_password(tokens[2]) || !check_only_digits(tokens[1]) ||
        !check_only_alphanumerical(tokens[2]))
    {
        msg = "RUR ERR\n";
        return msg;
    }
    switch (check_unregister(tokens[1], tokens[2]))
    {
    case OK:
    {
        msg = "RUR OK\n";
        return msg;
    }
    case NOK:
    {
        msg = "RUR NOK\n";
        return msg;
    }
    case UNR:
    {
        msg = "RUR UNR\n";
        return msg;
    }
    case WRP:
    {
        msg = "RUR WRP\n";
        return msg;
    }
    }
    return "ERR\n";
}

string handle_changePass(vector<string> tokens)
{
    string msg;
    if (tokens.size() != 4 || !check_size_uid(tokens[1]) || !check_only_digits(tokens[1]) ||
        !check_size_password(tokens[2]) || !check_size_password(tokens[3]) ||
        !check_only_alphanumerical(tokens[2]) || !check_only_alphanumerical(tokens[3]))
    {
        msg = "RCP ERR\n";
        return msg;
    }

    switch (check_changePass(tokens[1], tokens[2], tokens[3]))
    {
    case OK:
    {
        msg = "RCP OK\n";
        return msg;
    }
    case NLG:
    {
        msg = "RCP NLG\n";
        return msg;
    }
    case NOK:
    {
        msg = "RCP NOK\n";
        return msg;
    }
    case NID:
    {
        msg = "RCP NID\n";
        return msg;
    }
    }
    return "ERR\n";
}

/**
 * Create command.
 * OK - event created successfully
 * NOK - event could not be created
 * NLG - user was not logged in
 * WRP - incorrect password
 */
string handle_create(vector<string> tokens)
{
    string file_content, response, msg, status, reply_command, eid, date;

    if (tokens.size() != 10)
    {
        msg = "RCE ERR\n";
        return msg;
    }

    date = tokens[4] + ' ' + tokens[5];

    if (!check_size_uid(tokens[1]) ||
        !check_size_password(tokens[2]) || !check_only_digits(tokens[1]) ||
        !check_only_alphanumerical(tokens[2]) ||
        !check_size_event_name(tokens[3]) || !check_only_alphanumerical(tokens[3]) ||
        !check_only_digits(tokens[6]) || stoi(tokens[6]) < 10 || stoi(tokens[6]) > 999 ||
        !check_size_file_name(tokens[7]) || !check_file_name(tokens[7]) ||
        !check_hour(tokens[5]) || !check_date(tokens[4]) ||
        !check_future_date(date) ||
        !check_only_digits(tokens[8]) || !check_size_file(stoi(tokens[8])) ||
        tokens[9].size() != stoul(tokens[8]))
    {
        msg = "RCE ERR\n";
        return msg;
    }

    switch (check_create_event(tokens[1], tokens[2], tokens[3],
                               date, tokens[6], tokens[7], tokens[8], tokens[9], eid))
    {
    case 0:
    {
        msg = "RCE OK " + eid + "\n";
        return msg;
    }
    case 1:
    {
        msg = "RCE NOK\n";
        return msg;
    }
    case 2:
    {
        msg = "RCE NLG\n";
        return msg;
    }
    default:
    {
        msg = "RCE WRP\n";
        return msg;
    }
    }
    return "ERR\n";
}

string handle_close_event(vector<string> tokens)
{
    string msg;
    if (tokens.size() != 4 || !check_size_uid(tokens[1]) || !check_only_digits(tokens[1]) ||
        !check_size_password(tokens[2]) || !check_size_eid(tokens[3]) ||
        !check_only_alphanumerical(tokens[2]) || !check_only_digits(tokens[3]))
    {
        msg = "RCL ERR\n";
        return msg;
    }
    switch (check_close_event(tokens[1], tokens[2], tokens[3]))
    {
    case 0:
    {
        msg = "RCL OK\n";
        return msg;
    }
    case 1:
    {
        msg = "RCL NLG\n";
        return msg;
    }
    case 2:
    {
        msg = "RCL NOK\n";
        return msg;
    }
    case 3:
    {
        msg = "RCL NOE\n";
        return msg;
    }
    case 4:
    {
        msg = "RCL EOW\n";
        return msg;
    }
    case 5:
    {
        msg = "RCL SLD\n";
        return msg;
    }
    case 6:
    {
        msg = "RCL PST\n";
        return msg;
    }
    default:
    {
        msg = "RCL CLO\n";
        return msg;
    }
    }
    return "ERR\n";
}

string handle_myevents(vector<string> tokens)
{
    string msg, events;
    if (tokens.size() != 3 || !check_size_uid(tokens[1]) ||
        !check_size_password(tokens[2]) || !check_only_digits(tokens[1]) ||
        !check_only_alphanumerical(tokens[2]))
    {
        msg = "RME ERR\n";
        return msg;
    }
    switch (check_myevents(tokens[1], tokens[2], events))
    {
    case OK:
    {
        msg = "RME OK" + events + "\n";
        return msg;
    }
    case NOK:
    {
        msg = "RME NOK\n";
        return msg;
    }
    case NLG:
    {
        msg = "RME NLG\n";
        return msg;
    }
    case WRP:
    {
        msg = "RME WRP\n";
        return msg;
    }
    }
    return "ERR\n";
}

string handle_list(vector<string> tokens)
{
    string msg, events;
    if (tokens.size() != 1)
    {
        msg = "RLS ERR\n";
        return msg;
    }
    switch (check_list(events))
    {
    case OK:
    {
        msg = "RLS OK" + events + "\n";
        return msg;
    }
    case NOK:
    {
        msg = "RLS NOK\n";
        return msg;
    }
    }
    return "ERR\n";
}

string handle_show(vector<string> tokens)
{
    string msg, event;
    if (tokens.size() != 2 || !check_size_eid(tokens[1]) ||
        !check_only_digits(tokens[1]))
    {
        msg = "RSE ERR\n";
        return msg;
    }

    switch (check_show(tokens[1], event))
    {
    case OK:
    {
        msg = "RSE OK " + event + "\n";
        return msg;
    }
    case NOK:
    {
        msg = "RSE NOK\n";
        return msg;
    }
    }
    return "ERR\n";
}

string handle_reserve(vector<string> tokens)
{
    string response, msg, status, reply_command;
    vector<string> divided_response;
    string remaining_seats;
    int case_rej;
    if (tokens.size() != 5 || !check_size_uid(tokens[1]) ||
        !check_size_password(tokens[2]) || !check_only_digits(tokens[1]) ||
        !check_only_alphanumerical(tokens[2]) || !check_size_eid(tokens[3]) ||
        !check_only_digits(tokens[3]) || !check_only_digits(tokens[4]) ||
        stoi(tokens[4]) < 1 || stoi(tokens[4]) > 999)
    {
        msg = "RRI ERR\n";
        return msg;
    }
    switch (check_reserve(tokens[1], tokens[2], tokens[3], stoi(tokens[4]), case_rej))
    {
    case 0:
    {
        msg = "RRI ACC\n";
        return msg;
    }
    case 1:
    {
        msg = "RRI NOK\n";
        return msg;
    }
    case 2:
    {
        msg = "RRI NLG\n";
        return msg;
    }
    case 3:
    {
        msg = "RRI SLD\n";
        return msg;
    }
    case 4:
    {
        msg = "RRI REJ " + to_string(case_rej) + "\n";
        return msg;
    }
    case 5:
    {
        msg = "RRI WRP\n";
        return msg;
    }
    case 6:
    {
        msg = "RRI PST\n";
        return msg;
    }
    default:
    {
        msg = "RRI CLO\n";
        return msg;
    }
    }
    return "ERR\n";
}

string handle_myreservations(vector<string> tokens)
{
    string response, msg, status, reply_command, myr;
    vector<string> divided_response;
    if (tokens.size() != 3 || !check_size_uid(tokens[1]) ||
        !check_size_password(tokens[2]) || !check_only_digits(tokens[1]) ||
        !check_only_alphanumerical(tokens[2]))
    {
        msg = "RMR ERR\n";
        return msg;
    }

    switch (check_myreservations(tokens[1], tokens[2], myr))
    {
    case 0:
    {
        msg = "RMR OK" + myr + "\n";
        return msg;
    }
    case 1:
    {
        msg = "RMR NOK\n";
        return msg;
    }
    case 2:
    {
        msg = "RMR NLG\n";
        return msg;
    }
    default:
    {
        msg = "RMR WRP\n";
        return msg;
    }
    }
    return "ERR\n";
}