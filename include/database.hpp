#ifndef DATABASE_HPP
#define DATABASE_HPP

#include <string>
#include <vector>

using namespace std;

int create_data_base();

void create_user_dir(string uid, string password);

void login_user(string uid);

void logout_user(string uid);

int dir_exists(string path);

bool check_user_registered(string uid);

bool check_user_login(string uid);

void erase_user_dir(string uid);

void change_pass(string uid, string password);

string get_password(string uid);

string get_user_events(string uid);

string get_user_reservations();

string create_event_dir(string uid, string event_name, string event_date,
                        string attendance_size, string file_name,
                        string file_size, string file_content);

int get_number_reservations();

string get_event_list();

string get_event_info(string event_id);

void reserve_event(string uid, string eid, string num_seats);

int get_available_seats(string event_id);

void close_event(string eid);

bool check_event_existance(string eid);

bool check_event_closed(string event_id);

bool check_event_future(string event_id);

bool check_event_available_seats(string event_id);

bool check_is_user_event(string username, string eid)

#endif