#ifndef PROTOCOL_HPP
#define PROTOCOL_HPP

#include <vector>

using namespace std;

/// handles the input
/// @param argc
/// @param argv
void handle_ip_port(int argc, char **argv);

/// handles login request
/// @param token
void handle_login(vector<string> tokens);

/// handles change pass request
/// @param token
void handle_changePass(vector<string> tokens);

/// handles unregister request
/// @param token
void handle_unregister(vector<string> tokens);

/// handles logout request
/// @param token
void handle_logout(vector<string> tokens);

/// handles exit request
/// @param token
bool handle_exit_user(vector<string> tokens);

/// handles create request
/// @param token
void handle_create(vector<string> tokens);

/// handles close event request
/// @param token
void handle_close_event(vector<string> tokens);

/// handles myevents request
/// @param token
void handle_myevents(vector<string> tokens);

/// handles list request
/// @param token
void handle_list(vector<string> tokens);

/// handles show request
/// @param token
void handle_show(vector<string> tokens);

/// handles reserve request
/// @param token
void handle_reserve(vector<string> tokens);

/// handles myreservations request
/// @param token
void handle_myreservations(vector<string> tokens);

#endif