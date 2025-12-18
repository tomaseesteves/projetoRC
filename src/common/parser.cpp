#include <iostream>
#include <string.h>
#include <vector>
#include <sstream>
#include <stdlib.h>
#include <iterator>
#include <unistd.h>

#include "parser.hpp"

using namespace std;

Command_Options resolve_command(string command)
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

vector<string> split_string(string &input)
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

string resolve_state(string state) 
{
    switch(stoi(state)) 
    {
        case 0: return "Event has already happened";
        case 1: return "Still accepting reservations";
        case 2: return "Event has sold out all its seats";
        case 3: return "Closed";
    }
    return "invalid state";
}

string trim(string &s) 
{
    while (!s.empty() && (s[s.length()-1] == '\n' || s[s.length()-1] == '\r')) {
        s.erase(s.length()-1);
    }
    return s;
}

size_t split_nth_space(string &s, int n)
{
    size_t pos = -1;
    while (n-- > 0)
    {
        pos = s.find(' ', pos + 1);
        if (pos == string::npos) break;
    }
    return pos;
}

vector<string> extract_file_data(string &s, int create_flag)
{
    vector<string> tokens;
    size_t first_split = -1;
    size_t second_split = -1;

    // If given string corresponds to a Create command
    if(create_flag)
    {
        first_split = split_nth_space(s, 3);
        second_split = split_nth_space(s, 9);

        string user_data = s.substr(0, first_split);
        string file_data = s.substr(first_split + 1, second_split - first_split - 1);
        string file_content = s.substr(second_split + 1);
        tokens.push_back(user_data);
        tokens.push_back(file_data);
        tokens.push_back(file_content);

        return tokens;
    }
    // If given string corresponds to a Show response
    first_split = split_nth_space(s, 2);
    second_split = split_nth_space(s, 10);

    string response_data = s.substr(0, first_split);
    string file_data = s.substr(first_split + 1, second_split - first_split - 1);
    string file_content = s.substr(second_split + 1);
    tokens.push_back(response_data);
    tokens.push_back(file_data);
    tokens.push_back(file_content);

    return tokens;
}
