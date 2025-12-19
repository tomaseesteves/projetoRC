#ifndef SERVER_HPP
#define SERVER_HPP

#include <string>

using namespace std;

/// Parses requests sent to server
/// @param full_msg
/// @return server response
string handle_request(string full_msg);

#endif