#ifndef USER_NETWORK_HPP
#define USER_NETWORK_HPP

using namespace std;

/// Connection with UPD server.
/// @param ip_address
/// @param port
/// @param msg
/// @return response
string connect_UDP(char *ip_address, char *port, string msg);

/// Connection with TCP server.
/// @param ip_address
/// @param port
/// @param msg
/// @return response
string connect_TCP(char *ip_address, char *port, string msg);

#endif