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

Command_Options resolveServerRequest(string command);

/// Identify what command the user has just sent.
/// @param command
/// @return enum value
Command_Options resolve_command(string command);

/// Splits given string into a vector, using " " as a delimiter.
/// @param input
/// @return tokens
vector<string> split_string(string &input);

/// Parses meaning of state received in commands 'list' and 'myevents'
/// @param state
/// @return state of event
string resolve_state(string state);

/// Trims newline from end of string
/// @param s
/// @return s without '\n' at the end
string trim(string &s);

/// Finds nth occurence of ' ' char in a string
/// @param s
/// @param n
/// @return position of nth ' ' char
size_t split_nth_space(string &s, int n);

/// Separates file data from its content contained within a string
/// @param s
/// @param command_flag
/// @return tokens
vector<string> extract_file_data(string &s, int command_flag);

#endif