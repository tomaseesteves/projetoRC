#ifndef UTILS_HPP
#define UTILS_HPP

using namespace std;

/// checks if the uid size is 6
/// @param uid
/// @return bool
bool check_size_uid(string &uid);

/// checks if eid size is 3
/// @param eid
/// @return bool
bool check_size_eid(string &eid);

/// checks if the password size is 8
/// @param password
/// @return bool
bool check_size_password(string &password);

/// checks if the password size is up to 24
/// @param s
/// @return bool
bool check_size_file_name(string &s);

/// checks if the string only contains digits
/// @param s
/// @return bool
bool check_only_digits(string &s);

/// checks if the string only contains digits and letters
/// @param s
/// @return bool
bool check_only_alphanumerical(string &s);

/// checks the size of the event name
/// @param s
/// @return bool
bool check_size_event_name(string &s);

/// checks if date is valid
/// @param s
/// @return bool
bool check_date(string &s);

/// checks if file name only contains alphanumerics, dots or underscores
/// @param s
/// @return bool
bool check_file_name(string &s);

#endif