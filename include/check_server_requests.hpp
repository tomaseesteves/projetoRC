#ifndef CHECK_SERVER_REQUESTS_HPP
#define CHECK_SERVER_REQUESTS_HPP

using namespace std;

/// checks the login
/// @param username
/// @param password
/// @return the status
int check_login(string username, string password);

/// checks the logout
/// @param username
/// @param password
/// @return the status
int check_logout(string username, string password);

/// checks the register
/// @param username
/// @param password
/// @return the status
int check_unregister(string username, string password);

/// checks change pass
/// @param username
/// @param password_old
/// @param password_new
/// @return the status
int check_changePass(string username, string password_old, string password_new);

int check_myevents(string username, string password, string &msg);

int check_close_event(string username, string password, string eid);

int check_list(string &msg);

int check_show(string eid, string &msg);

int check_myreservations(string username, string password, string &msg);

int check_reserve(string username, string password, string eid, int value, int &msg);

int check_create_event(string username, string password, string event_name, string event_date,
                       string attendance_size, string file_name,
                       string file_size, string file_content, string &eid);

#endif