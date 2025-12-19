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
#include <algorithm>

#include <string>
#include <vector>
#include <dirent.h>

#include <utils.hpp>
#include <parser.hpp>
#include <database_utils.hpp>

using namespace std;

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
    return "ESDIR/USERS/" + UID + "/CREATED/" + EID + ".txt";
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

string sort_reservations(vector<Reservation> r)
{
    string response = "";

    if (r.empty())
        return response;

    // Sort by date, keep 50 most recent reservations
    sort(r.begin(), r.end(),
         [](const Reservation &a, const Reservation &b)
         {
             return a.date_key > b.date_key;
         });
    if (r.size() > 50)
        r.resize(50);

    // Sort by eventID
    sort(r.begin(), r.end(),
         [](const Reservation &a, const Reservation &b)
         {
             return a.event_id < b.event_id;
         });

    for (Reservation res : r)
    {
        response += " " + res.event_id + " " + res.date_str + " " + res.num_seats;
    }
    return response;
}