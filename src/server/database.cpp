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

#include <string>
#include <vector>
#include <dirent.h>

#include <file.hpp>
#include <utils.hpp>
#include <parser.hpp>

using namespace std;

/// devia-se fazer exit caso nao consigo criar dir do user?
/// nao deveria existir locks?
/// ver o network user
/// temos de por verificaçao de future events

string give_user_dir(string UID)
{
    return "ESDIR/USERS/" + UID;
}

string give_user_login_file(string UID)
{
    return "ESDIR/USERS/" + UID + "/" + UID + "_login.txt";
}

string give_user_password_file(string UID)
{
    return "ESDIR/USERS/" + UID + "/" + UID + "_pass.txt";
}

string give_user_created_dir(string UID)
{
    return "ESDIR/USERS/" + UID + "/CREATED";
}

string give_user_reserved_dir(string UID)
{
    return "ESDIR/USERS/" + UID + "/RESERVED";
}

string give_user_event_file(string UID, string EID)
{
    return "ESDIR/USERS/" + UID + "/RESERVED/" + EID + ".txt";
}

string give_event_dir(string EID)
{
    return "ESDIR/EVENTS/" + EID;
}

string give_event_start_file(string EID)
{
    return "ESDIR/EVENTS/" + EID + "/START_" + EID + ".txt";
}

string give_event_available_file(string EID)
{
    return "ESDIR/EVENTS/" + EID + "/AVAILABLE_" + EID + ".txt";
}

string give_event_description_dir(string EID)
{
    return "ESDIR/EVENTS/" + EID + "/DESCRIPTION";
}

string give_event_end_file(string EID)
{
    return "ESDIR/EVENTS/" + EID + "/END_" + EID + ".txt";
}

string give_event_reservations_dir(string EID)
{
    return "ESDIR/EVENTS/" + EID + "/RESERVATIONS";
}

int create_data_base()
{
    if (mkdir("ESDIR", 0700) == -1)
    {
        if (errno != EEXIST)
        {
            exit(1);
        }
    }
    if (mkdir("ESDIR/USERS", 0700) == -1)
    {
        if (errno != EEXIST)
        {
            exit(1);
        }
    }
    if (mkdir("ESDIR/EVENTS", 0700) == -1)
    {
        if (errno != EEXIST)
        {
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
    size_t pass_len = password.size(), total_bytes;
    if (mkdir(uid_dir.c_str(), 0700) == -1)
    {
        if (errno != EEXIST)
        {
            exit(1);
        }
    }
    if (mkdir(created.c_str(), 0700) == -1)
    {
        if (errno != EEXIST)
        {
            exit(1);
        }
    }
    if (mkdir(reserved.c_str(), 0700) == -1)
    {
        if (errno != EEXIST)
        {
            exit(1);
        }
    }

    fd = open(pass.c_str(), O_WRONLY | O_CREAT, 0700);
    if (fd == -1)
    {
        exit(1);
    }
    while (total_bytes < pass_len)
    {
        n = write(fd, password.c_str() + total_bytes, pass_len - total_bytes);
        if (n <= 0)
        {
            exit(1);
        }
        total_bytes += n;
    }

    close(fd);
    fd = open(login.c_str(), O_WRONLY | O_CREAT, 0700);
    if (fd == -1)
    {
        exit(1);
    }
    while (total_bytes < 1)
    {
        n = write(fd, "1" + total_bytes, 1 - total_bytes);
        if (n <= 0)
        {
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
    size_t total_bytes;
    fd = open(login.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0700);
    if (fd == -1)
    {
        exit(1);
    }
    while (total_bytes < 1)
    {
        n = write(fd, "1" + total_bytes, 1 - total_bytes);
        if (n <= 0)
        {
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
    size_t total_bytes;
    fd = open(login.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0700);
    if (fd == -1)
    {
        exit(1);
    }
    while (total_bytes < 1)
    {
        n = write(fd, "0" + total_bytes, 1 - total_bytes);
        if (n <= 0)
        {
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
    return access(password.c_str(), R_OK | W_OK);
}

bool check_user_login(string uid)
{
    string login = give_user_login_file(uid);
    int fd = open(login.c_str(), O_RDONLY);
    ssize_t n;
    size_t total_bytes;
    int buf[1];
    if (fd == -1)
    {
        exit(1);
    }
    while (total_bytes < 1)
    {
        n = read(fd, buf + total_bytes, 1 - total_bytes);
        if (n <= 0)
        {
            exit(1);
        }
        total_bytes += n;
    }
    close(fd);
    return buf[1] ? true : false;
}

void erase_user_dir(string uid)
{
    string userdir = give_user_dir(uid),
           pass = give_user_password_file(uid),
           login = give_user_login_file(uid);
    if (unlink(pass.c_str()) == -1 || unlink(login.c_str()))
    {
        exit(1);
    }
}

void change_pass(string uid, string password)
{
    int fd;
    string pass_path = give_user_password_file(uid);
    ssize_t n, pass_len = password.size();
    size_t total_bytes;
    fd = open(pass_path.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0700);
    if (fd == -1)
    {
        exit(1);
    }
    while (total_bytes < pass_len)
    {
        n = write(fd, password.c_str() + total_bytes, pass_len - total_bytes);
        if (n <= 0)
        {
            exit(1);
        }
        total_bytes += n;
    }
    close(fd);
}

string get_password(string uid)
{
    string pass_path = give_user_password_file(uid);
    int fd = open(pass_path.c_str(), O_RDONLY);
    ssize_t n;
    size_t total_bytes;
    string password;
    if (fd == -1)
    {
        exit(1);
    }
    while (total_bytes < 8)
    {
        n = read(fd, &password[total_bytes], 8 - total_bytes);
        if (n <= 0)
        {
            exit(1);
        }
        total_bytes += n;
    }
    close(fd);
    return password;
}

string get_user_events(string uid)
{
    string created_path = give_user_created_dir(uid),
           eid, user_events = "", event_path_available, spaces_left,
           status, event_path_end, event_path_start, date,
           start_content;
    int start_size;
    DIR *dir = opendir(created_path.c_str());
    struct dirent *entry;
    size_t total_bytes;
    ssize_t n;
    int fd;

    if (!dir)
    {
        exit(1);
    }

    while ((entry = readdir(dir)) != NULL)
    {
        if (strcmp(entry->d_name, ".") == 0 ||
            strcmp(entry->d_name, "..") == 0)
            continue;
        // eid = strrchr(entry->d_name, '.');
        eid = strtok(entry->d_name, ".");
        event_path_available = give_event_available_file(eid);
        event_path_end = give_event_end_file(eid);
        event_path_start = give_event_start_file(eid);

        fd = open(event_path_start.c_str(), O_RDONLY);
        if (fd == -1)
        {
            exit(1);
        }
        extract_file(event_path_start, start_content, start_size);

        close(fd);
        if (!check_future_date(date))
        {
            status = '0';
        }
        else if (access(event_path_end.c_str(), F_OK) == 0)
        {
            status = '3';
        }
        else
        {
            fd = open(event_path_res.c_str(), O_RDONLY);
            if (fd == -1)
            {
                exit(1);
            }
            while (total_bytes < 3)
            {
                n = read(fd, &spaces_left[total_bytes], 3 - total_bytes);
                if (n <= 0)
                {
                    exit(1);
                }
                total_bytes += n;
            }
            if (stoi(spaces_left) != 0)
            {
                status = '1';
            }
            else
            {
                status = '2';
            }
            user_events += eid + ' ' + status + '\n';
            close(fd);
        }
        user_events += eid + ' ' + status + '\n';
    }
    closedir(dir);
    return user_events;
}

string create_event_dir(string uid, string event_name, string event_date,
                        string attendance_size, string file_name,
                        string file_size, string file_content)
{
    string all_events = "ESDIR/EVENTS", event_id = "", event_dir, user_file,
           start_file, available_file, description_dir, description_file,
           reservations_dir, start_content;
    int highest_id = 0, fd, n;
    size_t total_bytes = 0, start_len;
    DIR *dir = opendir(all_events.c_str());
    struct dirent *entry;

    while ((entry = readdir(dir)) != NULL)
    {
        if (strcmp(entry->d_name, ".") == 0 ||
            strcmp(entry->d_name, "..") == 0)
            continue;
        if (atoi(entry->d_name) > highest_id)
        {
            highest_id = atoi(entry->d_name);
        }
        if (highest_id > 999)
        {
            return event_id; // nao pode haver mais de 999 eventos!
        }
    }

    event_id = to_string(highest_id);
    event_dir = give_event_dir(event_id);
    user_file = give_user_created_dir(uid) + "/" + event_id + ".txt";
    start_file = give_event_start_file(event_id);
    available_file = give_event_available_file(event_id);
    description_dir = give_event_description_dir(event_id);
    description_file = description_dir + "/" + file_name;
    reservations_dir = give_event_reservations_dir(event_id);

    // Create user file
    fd = open(user_file.c_str(), O_WRONLY | O_CREAT, 0700);
    if (fd == -1)
    {
        exit(1);
    }
    close(fd);

    // Create event directory
    if (mkdir(event_dir.c_str(), 0700) == -1)
    {
        if (errno != EEXIST)
        {
            exit(1);
        }
    }

    // Create START_(eid).txt file
    start_content = uid + " " + event_name + " " + file_name + " " +
                    attendance_size + " " + event_date;
    save_event_file(start_file, start_content, (uint64_t)start_content.length());

    // Create RES_(eid).txt file
    save_event_file(available_file, attendance_size, (uint64_t)attendance_size.length());

    // Create DESCRIPTION directory
    if (mkdir(description_dir.c_str(), 0700) == -1)
    {
        if (errno != EEXIST)
        {
            exit(1);
        }
    }

    // Create event file given by client
    save_event_file(description_file, file_content, stoull(file_size));

    // Create RESERVATIONS directory
    if (mkdir(reservations_dir.c_str(), 0700) == -1)
    {
        if (errno != EEXIST)
        {
            exit(1);
        }
    }
    return event_id;
}

// REVER
string get_user_reservations(string event_id)
{
    string res_dir = give_event_reservations_dir(event_id);
    int total_reservations = 0, event_reservations, event_file, fd, n;
    vector<string> divided_file;
    size_t total_bytes;
    char *buffer[24];
    DIR *dir = opendir(res_dir.c_str());
    struct dirent *entry;

    while ((entry = readdir(dir)) != NULL)
    {
        if (strcmp(entry->d_name, ".") == 0 ||
            strcmp(entry->d_name, "..") == 0)
            continue;

        fd = open(entry->d_name, O_RDONLY);
        if (fd == -1)
        {
            exit(1);
        }
        while (total_bytes < 24)
        {
            n = read(fd, buffer, 24 - total_bytes);
            if (n <= 0)
            {
                exit(1);
            }
            total_bytes += n;
        }
        divided_file = split_string(string(buffer)); // importar!!!
        event_reservations = stoi(divided_file[1]);
        total_reservations += event_reservations;
        close(fd);
    }
    return total_reservations;
}

string get_event_list()
{
    string all_events = "ESDIR/EVENTS", response;
    DIR *dir = opendir(all_events.c_str());
    struct dirent *entry;

    while ((entry = readdir(dir)) != NULL)
    {
        if (strcmp(entry->d_name, ".") == 0 ||
            strcmp(entry->d_name, "..") == 0)
            continue;

        string event_id = string(entry->d_name);
        string event_path = all_events + "/" + event_id;

        // Open directory of an event
        DIR *dir = opendir(entry->d_name);
        struct dirent *entry_event;

        while ((entry_event = readdir(dir)) != NULL)
        {
            string status;
            string start_file = give_event_start_file(event_id);
            string available_file = give_event_available_file(event_id);
            string end_file = give_event_end_file(event_id);

            // Extract START file content
            string start_content;
            int start_size;
            extract_file(start_file, start_content, start_size);
            vector<string> divided_start_file = split_string(start_content);
            string event_date = divided_start_file[4] + " " + divided_start_file[5];
            string event_name = divided_start_file[1];
            int attendance_size = stoi(divided_start_file[3]);

            // Extract AVAILABLE file content
            string available_content;
            int available_size;
            extract_file(available_file, available_content, available_size);
            int num_seats_available = stoi(available_content);

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
            response += " " + event_id + " " + event_name + " " + status + " " + event_date;
        }
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

    extract_file(start_file, start_content, start_size);
    vector<string> divided_start = split_string(start_content);
    string user_id = divided_start[0];
    string event_name = divided_start[1];
    string event_date = divided_start[4] + " " + divided_start[5];
    string file_name = divided_start[2];
    int attendance_size = stoi(divided_start[3]);

    extract_file(available_file, available_content, available_size);
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

    string res_file = "R-" + uid + "-" + time1 + ".txt";
    string user_res_file = give_user_reserved_dir(uid) + "/" + res_file;
    string event_res_file = give_event_reservations_dir(eid) + "/" + res_file;
    string res_content = uid + " " + num_seats + " " + time2;
    save_event_file(user_res_file, res_content, (uint64_t)res_content.length());
    save_event_file(event_res_file, res_content, (uint64_t)res_content.length());

    string available_file = give_event_available_file(eid);
    int new_avail_seats = get_available_seats(eid) - stoi(num_seats);
    string avail_content = to_string(new_avail_seats);
    save_event_file(available_file, avail_content, (uint64_t)avail_content.length());
}

void close_event(string eid)
{
    int fd;
    string close_eid_file = give_event_end_file(eid);
    fd = open(close_eid_file.c_str(), O_WRONLY | O_CREAT, 0700);
    if (fd == -1)
    {
        exit(1);
    }
}

bool check_event_existence(string eid)
{
    return access(give_event_dir(eid).c_str(), R_OK | W_OK) != 0;
}

bool check_is_user_event(string username, string eid)
{
    return access(give_user_event_file(username, eid).c_str(), R_OK | W_OK) != 0;
}

int get_available_seats(string event_id)
{
    string available_file = give_event_available_file(event_id);

    // Extract AVAILABLE file content
    string available_content;
    int available_size;
    extract_file(available_file, available_content, available_size);

    return stoi(available_content);
}

bool check_event_closed(string event_id)
{
    string end_file = give_event_end_file(event_id);
    return access(end_file.c_str(), F_OK) != 0;
}

bool check_event_future(string event_id)
{
    string start_file = give_event_start_file(event_id), start_content;
    int start_size;
    extract_file(start_file, start_content, start_size);
    vector<string> divided_start_file = split_string(start_content);
    string event_date = divided_start_file[4] + " " + divided_start_file[5];

    return check_future_date(event_date);
}

bool check_event_available_seats(string event_id)
{
    return get_available_seats(event_id) > 0;
}
