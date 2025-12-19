#ifndef DATABASE_UTILS_HPP
#define DATABASE_UTILS_HPP

#include <string>

using namespace std;

struct Reservation
{
    string event_id;
    string date_str; // for printing: dd-mm-YYYY HH:MM:SS
    string date_key; // for sorting: YYYYmmdd_HHMMSS
    string num_seats;
};

/// Return path to user directory
/// @param UID
/// @return string
string give_user_dir(string UID);

/// Return file name of user login file
/// @param UID
/// @return string
string give_user_login_file(string UID);

/// Return file name of user password file
/// @param UID
/// @return string
string give_user_password_file(string UID);

/// Return path to user created events directory
/// @param UID
/// @return string
string give_user_created_dir(string UID);

/// Return path to user reserved events directory
/// @param UID
/// @return string
string give_user_reserved_dir(string UID);

/// Return file name of user's created event file
/// @param UID
/// @param EID
/// @return
string give_user_event_file(string UID, string EID);

/// Return path to event directory
/// @param EID
/// @return
string give_event_dir(string EID);

/// Return file name of event start file
/// @param EID
/// @return
string give_event_start_file(string EID);

/// Return file name of event availability file
/// @param EID
/// @return
string give_event_available_file(string EID);

/// Return path to event description directory
/// @param EID
/// @return
string give_event_description_dir(string EID);

/// Return file name of event end file
/// @param EID
/// @return
string give_event_end_file(string EID);

/// Return path to event reservations directory
/// @param EID
/// @return
string give_event_reservations_dir(string EID);

/// Sort user reservations, first by date descending, then by eid
/// @param r 
/// @return string of sorted reservations
string sort_reservations(vector<Reservation> r);

#endif