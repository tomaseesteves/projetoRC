#include <iostream>
#include <string.h>
#include <vector>
#include <sstream>
#include <stdlib.h>
#include <iterator>
#include <unistd.h>

#include "parser.hpp"

using namespace std;

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

string resolveState(string state) 
{
    switch(stoi(state)) 
    {
        case 0: return "event has already happened";
        case 1: return "still accepting reservations";
        case 2: return "event has sold out all its seats";
        case 3: return "closed";
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
