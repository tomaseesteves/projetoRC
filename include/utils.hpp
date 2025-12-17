#ifndef UTILS_HPP
#define UTILS_HPP

using namespace std;

/// Checks if the uid size is 6
/// @param uid
/// @return bool
bool check_size_uid(string &uid);

/// Checks if eid size is 3
/// @param eid
/// @return bool
bool check_size_eid(string &eid);

/// Checks if the password size is 8
/// @param password
/// @return bool
bool check_size_password(string &password);

/// Checks if the password size is up to 24
/// @param s
/// @return bool
bool check_size_file_name(string &s);

/// Checks the size of the event name
/// @param s
/// @return bool
bool check_size_event_name(string &s);

/// Checks a file's size
/// @param file_size 
/// @return bool
bool check_size_file(int file_size);

/// Checks if file's size has less than 8 digits
/// @param s 
/// @return bool
bool check_number_digits_file_size(string &s);

/// Checks if the string only contains digits
/// @param s
/// @return bool
bool check_only_digits(string &s);

/// Checks if the string only contains digits and letters
/// @param s
/// @return bool
bool check_only_alphanumerical(string &s);

/// Checks if date is valid
/// @param s
/// @return bool
bool check_date(string &s);

/// Checks if hour is valid
/// @param s
/// @return bool
bool check_hour(string &s);

/// Checks if date is in the future
/// @param s
/// @return bool
bool check_future_date(string &s);

/// Checks if file name only contains alphanumerics, dots or underscores
/// and if it has a valid file extension
/// @param s
/// @return bool
bool check_file_name(string &s);

#endif