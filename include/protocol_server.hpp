#ifndef PROTOCOL_SERVER_HPP
#define PROTOCOL_SERVER_HPP

#include <vector>
#include <string>

using namespace std;

/// Handles LIN request
/// @param tokens
/// @return
string handle_login(vector<string> tokens);

/// Handles LOU request
/// @param tokens
/// @return
string handle_logout(vector<string> tokens);

/// Handles UNR request
/// @param tokens
/// @return
string handle_unregister(vector<string> tokens);

/// Handles CPS request
/// @param tokens
/// @return
string handle_changePass(vector<string> tokens);

/// Handles CRE request
/// @param tokens
/// @return
string handle_create(vector<string> tokens);

/// handles CLS request
/// @param tokens
/// @return
string handle_close_event(vector<string> tokens);

/// Handles LME request
/// @param tokens
/// @return
string handle_myevents(vector<string> tokens);

/// Handle LST request
/// @param tokens
/// @return
string handle_list(vector<string> tokens);

/// Handles SED request
/// @param tokens
/// @return
string handle_show(vector<string> tokens);

/// Handle RID request
/// @param tokens
/// @return
string handle_reserve(vector<string> tokens);

/// Handle LMR request
/// @param tokens
/// @return
string handle_myreservations(vector<string> tokens);

#endif