#ifndef PARSER_HPP
#define PARSER_HPP

#include <vector>

using namespace std;

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

/// Identify what command the user has just sent.
/// @param command
/// @return enum value
Command_Options resolveCommand(string command);

/// Splits given string into a vector, using " " as a delimiter.
/// @param input
/// @return tokens
vector<string> splitString(string &input);

#endif