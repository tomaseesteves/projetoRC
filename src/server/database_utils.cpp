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

#include <utils.hpp>
#include <parser.hpp>
#include <database_utils.hpp>

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
