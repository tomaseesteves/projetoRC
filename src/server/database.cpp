#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <time.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <sys/stat.h>
#include <filesystem>
#include <ctime>
#include <iostream>

#include <string>
#include <vector>
#include <dirent.h>

#include <file.hpp>
#include <utils.hpp>
#include <parser.hpp>
#include <database.hpp>
#include <database_utils.hpp>
#include <algorithm>

using namespace std;

void create_data_base()
{
    if (mkdir("ESDIR", 0777) == -1)
    {
        if (errno != EEXIST)
        {
            perror("Error making ESDIR\n");
            exit(1);
        }
    }
    if (mkdir("ESDIR/USERS", 0777) == -1)
    {
        if (errno != EEXIST)
        {
            perror("Error making USERS DIR\n");
            exit(1);
        }
    }
    if (mkdir("ESDIR/EVENTS", 0777) == -1)
    {
        if (errno != EEXIST)
        {
            perror("Error making EVENTS DIR\n");
            exit(1);
        }
    }
}

void create_user_dir(string uid, string password)
{
    string uid_dir = give_user_dir(uid),
           pass = give_user_password_file(uid),
           login = give_user_login_file(uid),
           created = give_user_created_dir(uid),
           reserved = give_user_reserved_dir(uid);
    int fd;
    ssize_t n;
    size_t pass_len = password.size(), total_bytes = 0;
    if (mkdir(uid_dir.c_str(), 0777) == -1)
    {
        if (errno != EEXIST)
        {
            cout << "Error making USER " + uid + " DIR\n";
            exit(1);
        }
    }
    if (mkdir(created.c_str(), 0777) == -1)
    {
        if (errno != EEXIST)
        {
            cout << "Error making USER " + uid + " CREATED DIR\n";
            exit(1);
        }
    }
    if (mkdir(reserved.c_str(), 0777) == -1)
    {
        if (errno != EEXIST)
        {
            cout << "Error making USER " + uid + " RESERVED DIR\n";
            exit(1);
        }
    }

    fd = open(pass.c_str(), O_WRONLY | O_CREAT, 0777);
    if (fd == -1)
    {
        cout << "Error opening USER " + uid + " PASS FILE\n";
        exit(1);
    }
    while (total_bytes < pass_len)
    {
        n = write(fd, password.c_str() + total_bytes, pass_len - total_bytes);
        if (n < 0)
        {
            perror("write failed");
            exit(1);
        }
        total_bytes += n;
    }

    close(fd);
    fd = open(login.c_str(), O_WRONLY | O_CREAT, 0777);
    if (fd == -1)
    {
        cout << "Error opening USER " + uid + " LOGIN FILE\n";
        exit(1);
    }
    total_bytes = 0;
    while (total_bytes < 1)
    {
        n = write(fd, "1" + total_bytes, 1 - total_bytes);
        if (n < 0)
        {
            perror("write failed");
            exit(1);
        }
        total_bytes += n;
    }
    close(fd);
}

void login_user(string uid)
{
    int fd;
    string login = give_user_login_file(uid);
    ssize_t n;
    size_t total_bytes = 0;
    fd = open(login.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0777);
    if (fd == -1)
    {
        cout << "Error opening USER " + uid + " LOGIN FILE\n";
        exit(1);
    }
    while (total_bytes < 1)
    {
        n = write(fd, "1" + total_bytes, 1 - total_bytes);
        if (n < 0)
        {
            perror("write failed");
            exit(1);
        }
        total_bytes += n;
    }
    close(fd);
}

void logout_user(string uid)
{
    int fd;
    string login = give_user_login_file(uid);
    ssize_t n;
    size_t total_bytes = 0;
    fd = open(login.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0777);
    if (fd == -1)
    {
        cout << "Error opening USER " + uid + " LOGIN FILE\n";
        exit(1);
    }
    while (total_bytes < 1)
    {
        n = write(fd, "0" + total_bytes, 1 - total_bytes);
        if (n < 0)
        {
            perror("write failed");
            exit(1);
        }
        total_bytes += n;
    }
    close(fd);
}

int dir_exists(string path)
{
    struct stat info;
    return (stat(path.c_str(), &info) == 0 && S_ISDIR(info.st_mode));
}

bool check_user_registered(string uid)
{
    string password = give_user_password_file(uid);
    return !access(password.c_str(), R_OK | W_OK);
}

bool check_user_login(string uid)
{
    string login = give_user_login_file(uid), login_content;
    int login_size;
    if (!extract_file(login, login_content, login_size))
    {
        cout << "Error extracting file.\n";
        exit(1);
    }

    return login_content == "1";
}

void erase_user_dir(string uid)
{
    string userdir = give_user_dir(uid),
           pass = give_user_password_file(uid),
           login = give_user_login_file(uid);
    if (unlink(pass.c_str()) == -1 || unlink(login.c_str()))
    {
        perror("failed to delete login or pass file");
        exit(1);
    }
}

void change_pass(string uid, string password)
{
    int fd;
    string pass_path = give_user_password_file(uid);
    ssize_t n;
    size_t total_bytes = 0, pass_len = password.size();
    fd = open(pass_path.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0777);
    if (fd == -1)
    {
        cout << "Error opening USER " + uid + " PASS FILE\n";
        exit(1);
    }
    while (total_bytes < pass_len)
    {
        n = write(fd, password.c_str() + total_bytes, pass_len - total_bytes);
        if (n < 0)
        {
            perror("write failed");
            exit(1);
        }
        total_bytes += n;
    }
    close(fd);
}

string get_password(string uid)
{
    string pass_path = give_user_password_file(uid), password;
    int pass_size;
    if (!extract_file(pass_path, password, pass_size))
    {
        cout << "Error extracting file.\n";
        exit(1);
    }
    return password;
}

string get_user_events(string uid)
{
    string created_path = give_user_created_dir(uid),
           eid, user_events = "", event_path_available, spaces_left,
           status, event_path_end, event_path_start, date,
           start_content;
    vector<string> divided_res;
    int start_size;
    DIR *dir = opendir(created_path.c_str());
    struct dirent *entry;

    if (!dir)
    {
        perror("failed to open created user path dir\n");
        exit(1);
    }

    while ((entry = readdir(dir)) != NULL)
    {
        if (strcmp(entry->d_name, ".") == 0 ||
            strcmp(entry->d_name, "..") == 0)
            continue;

        eid = strtok(entry->d_name, ".");
        event_path_available = give_event_available_file(eid);
        event_path_end = give_event_end_file(eid);
        event_path_start = give_event_start_file(eid);

        if (!extract_file(event_path_start, start_content, start_size))
        {
            cout << "Error extracting file.\n";
            exit(1);
        }

        // Extract START file content
        if (!extract_file(event_path_start, start_content, start_size))
        {
            cout << "Error extracting file.\n";
            exit(1);
        }
        vector<string> divided_start_file = split_string(start_content);
        string event_date = divided_start_file[4] + " " + divided_start_file[5];

        int num_seats_available = get_available_seats(eid);

        if (!check_future_date(event_date))
        {
            status = "0"; // Event was in the past
        }
        else if (num_seats_available == 0)
        {
            status = "2"; // Event is sold out
        }
        else if (access(event_path_end.c_str(), F_OK) == 0)
        {
            status = "3"; // Event is closed
        }
        else
        {
            status = "1"; // Event is still accepting reservations
        }
        divided_res.push_back(' ' + eid + ' ' + status);
    }
    closedir(dir);

    // Sort and form full response
    sort(divided_res.begin(), divided_res.end());
    for (string res : divided_res)
    {
        user_events += res;
    }
    return user_events;
}

string create_event_dir(string uid, string event_name, string event_date,
                        string attendance_size, string file_name,
                        string file_size, string file_content)
{
    string all_events = "ESDIR/EVENTS", event_id = "", event_dir, user_file,
           start_file, available_file, description_dir, description_file,
           reservations_dir, start_content;
    int highest_id = 1, fd;
    char tmp[12];

    DIR *dir = opendir(all_events.c_str());
    struct dirent *entry;

    if (!dir)
    {
        perror("failed to open events dir\n");
        exit(1);
    }

    while ((entry = readdir(dir)) != NULL)
    {
        if (strcmp(entry->d_name, ".") == 0 ||
            strcmp(entry->d_name, "..") == 0)
            continue;
        if (atoi(entry->d_name) >= highest_id)
        {
            highest_id = atoi(entry->d_name) + 1;
        }
        if (highest_id > 999)
        {
            return event_id; // nao pode haver mais de 999 eventos!
        }
    }

    sprintf(tmp, "%03d", highest_id);
    event_id = tmp;
    event_dir = give_event_dir(event_id);
    user_file = give_user_created_dir(uid) + "/" + event_id + ".txt";
    start_file = give_event_start_file(event_id);
    available_file = give_event_available_file(event_id);
    description_dir = give_event_description_dir(event_id);
    description_file = description_dir + "/" + file_name;
    reservations_dir = give_event_reservations_dir(event_id);

    // Create user file
    fd = open(user_file.c_str(), O_WRONLY | O_CREAT, 0777);
    if (fd == -1)
    {
        cout << "Error making USER " + uid + " CREATED DIR\n";
        exit(1);
    }
    close(fd);

    // Create event directory
    if (mkdir(event_dir.c_str(), 0777) == -1)
    {
        if (errno != EEXIST)
        {
            printf("Error making Event Dir\n");
            exit(1);
        }
    }

    // Create START_(eid).txt file
    start_content = uid + " " + event_name + " " + file_name + " " +
                    attendance_size + " " + event_date;
    if (!save_event_file(start_file, start_content, (uint64_t)start_content.length()))
    {
        cout << "Error saving file.\n";
        exit(1);
    }

    // Create RES_(eid).txt file
    if (!save_event_file(available_file, attendance_size, (uint64_t)attendance_size.length()))
    {
        cout << "Error saving file.\n";
        exit(1);
    }

    // Create DESCRIPTION directory
    if (mkdir(description_dir.c_str(), 0777) == -1)
    {
        if (errno != EEXIST)
        {
            printf("Error making event description dir\n");
            exit(1);
        }
    }

    // Create event file given by client
    if (!save_event_file(description_file, file_content, stoull(file_size)))
    {
        cout << "Error saving file.\n";
        exit(1);
    }

    // Create RESERVATIONS directory
    if (mkdir(reservations_dir.c_str(), 0777) == -1)
    {
        if (errno != EEXIST)
        {
            printf("Error making event reservations dir\n");
            exit(1);
        }
    }
    return event_id;
}

string get_user_reservations(string user_id)
{
    string user_res_dir = give_user_reserved_dir(user_id),
           reservation_content, response = "";
    vector<Reservation> reservations;
    int file_size;
    DIR *dir = opendir(user_res_dir.c_str());
    struct dirent *entry;

    if (!dir)
    {
        perror("failed to open user reservations dir\n");
        exit(1);
    }

    while ((entry = readdir(dir)) != NULL)
    {
        if (strcmp(entry->d_name, ".") == 0 ||
            strcmp(entry->d_name, "..") == 0)
            continue;

        Reservation r;
        r.event_id = string(entry->d_name).substr(2, 3);
        r.date_key = string(entry->d_name).substr(6, 15);

        string full_path = user_res_dir + "/" + string(entry->d_name);

        if (!extract_file(full_path, reservation_content, file_size))
        {
            cout << "Error extracting file.\n";
            exit(1);
        }
        vector<string> divided_res = split_string(reservation_content);
        r.num_seats = divided_res[1];
        r.date_str = divided_res[2] + " " + divided_res[3];

        reservations.push_back(r);
    }
    closedir(dir);
    response = sort_reservations(reservations);
    return response;
}

string get_event_list()
{
    string all_events = "ESDIR/EVENTS", response;
    vector<string> divided_res;
    DIR *dir = opendir(all_events.c_str());
    struct dirent *entry;

    if (!dir)
    {
        perror("failed to open events dir\n");
        exit(1);
    }

    while ((entry = readdir(dir)) != NULL)
    {
        if (strcmp(entry->d_name, ".") == 0 ||
            strcmp(entry->d_name, "..") == 0)
            continue;

        string event_id = string(entry->d_name);
        string event_path = all_events + "/" + event_id;

        string status;
        string start_file = give_event_start_file(event_id);
        string available_file = give_event_available_file(event_id);
        string end_file = give_event_end_file(event_id);

        // Extract START file content
        string start_content;
        int start_size;
        if (!extract_file(start_file, start_content, start_size))
        {
            cout << "Error extracting file.\n";
            exit(1);
        }
        vector<string> divided_start_file = split_string(start_content);
        string event_date = divided_start_file[4] + " " + divided_start_file[5];
        string event_name = divided_start_file[1];

        int num_seats_available = get_available_seats(event_id);

        if (!check_future_date(event_date))
        {
            status = "0"; // Event was in the past
        }
        else if (num_seats_available == 0)
        {
            status = "2"; // Event is sold out
        }
        else if (access(end_file.c_str(), F_OK) == 0)
        {
            status = "3"; // Event is closed
        }
        else
        {
            status = "1"; // Event is still accepting reservations
        }
        divided_res.push_back(" " + event_id + " " + event_name + " " + status + " " + event_date);
    }
    closedir(dir);

    // Sort and form full response
    sort(divided_res.begin(), divided_res.end());
    for (string res : divided_res)
    {
        response += res;
    }
    return response;
}

string get_event_info(string event_id)
{
    string event_dir = give_event_dir(event_id),
           start_file = give_event_start_file(event_id), start_content,
           available_file = give_event_available_file(event_id), available_content,
           description_dir = give_event_description_dir(event_id), file_content,
           response;
    int start_size, available_size, file_size, num_seats_reserved = 0;

    if (!extract_file(start_file, start_content, start_size))
    {
        cout << "Error extracting file.\n";
        exit(1);
    }
    vector<string> divided_start = split_string(start_content);
    string user_id = divided_start[0];
    string event_name = divided_start[1];
    string event_date = divided_start[4] + " " + divided_start[5];
    string file_name = divided_start[2];
    int attendance_size = stoi(divided_start[3]);

    if (!extract_file(available_file, available_content, available_size))
    {
        cout << "Error extracting file.\n";
        exit(1);
    }
    num_seats_reserved = attendance_size - stoi(available_content);

    string description_file = description_dir + "/" + file_name;
    if (!extract_file(description_file, file_content, file_size))
    {
        return response; // empty string indicates NOK status
    }

    response = user_id + " " + event_name + " " + event_date + " " +
               to_string(attendance_size) + " " +
               to_string(num_seats_reserved) + " " +
               file_name + " " + to_string(file_size) + " " + file_content;

    return response;
}

void reserve_event(string uid, string eid, string num_seats)
{
    time_t now = time(NULL);
    struct tm tm_info;
    localtime_r(&now, &tm_info);
    char time1[32], time2[32];
    strftime(time1, sizeof(time1), "%Y%m%d_%H%M%S", &tm_info);
    strftime(time2, sizeof(time2), "%d-%m-%Y %H:%M:%S", &tm_info);

    string res_file = "R-" + eid + "-" + time1 + ".txt";
    string user_res_file = give_user_reserved_dir(uid) + "/" + res_file;
    string event_res_file = give_event_reservations_dir(eid) + "/" + res_file;
    string res_content = uid + " " + num_seats + " " + time2;

    if (!save_event_file(user_res_file, res_content, (uint64_t)res_content.length()))
    {
        cout << "Error saving file.\n";
        exit(1);
    }

    if (!save_event_file(event_res_file, res_content, (uint64_t)res_content.length()))
    {
        cout << "Error saving file.\n";
        exit(1);
    }

    string available_file = give_event_available_file(eid);
    int new_avail_seats = get_available_seats(eid) - stoi(num_seats);
    string avail_content = to_string(new_avail_seats);
    if (!save_event_file(available_file, avail_content, (uint64_t)avail_content.length()))
    {
        cout << "Error saving file.\n";
        exit(1);
    }
}

void close_active_event(string eid)
{
    int fd;
    string close_eid_file = give_event_end_file(eid);
    fd = open(close_eid_file.c_str(), O_WRONLY | O_CREAT, 0777);
    if (fd == -1)
    {
        cout << "Error opening " + eid + " event close file\n";
        exit(1);
    }
}

bool check_event_existence(string eid)
{
    return access(give_event_dir(eid).c_str(), R_OK | W_OK) == 0;
}

bool check_is_user_event(string username, string eid)
{
    return access(give_user_event_file(username, eid).c_str(), R_OK | W_OK) == 0;
}

int get_available_seats(string event_id)
{
    string available_file = give_event_available_file(event_id);

    // Extract AVAILABLE file content
    string available_content;
    int available_size;
    if (!extract_file(available_file, available_content, available_size))
    {
        cout << "Error extracting file.\n";
        exit(1);
    }

    return stoi(available_content);
}

bool check_event_closed(string event_id)
{
    string end_file = give_event_end_file(event_id);
    return access(end_file.c_str(), F_OK) == 0;
}

bool check_event_future(string event_id)
{
    string start_file = give_event_start_file(event_id), start_content;
    int start_size;
    if (!extract_file(start_file, start_content, start_size))
    {
        cout << "Error extracting file.\n";
        exit(1);
    }
    vector<string> divided_start_file = split_string(start_content);
    string event_date = divided_start_file[4] + " " + divided_start_file[5];

    return check_future_date(event_date);
}

bool check_event_available_seats(string event_id)
{
    return get_available_seats(event_id) > 0;
}
