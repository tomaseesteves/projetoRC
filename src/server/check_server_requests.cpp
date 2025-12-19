#include <iostream>
#include <vector>
#include <iterator>
#include <sstream>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#include <check_server_requests.hpp>
#include <database.hpp>
#include <parser.hpp>

using namespace std;

/// 0 caso que login deu bem
/// 1 caso que o username e a palavra passe estao incorretas
/// 2 caso nao existe esse user
int check_login(string username, string password)
{
    if (!check_user_registered(username))
    {
        create_user_dir(username, password);
        return REG;
    }
    else if (get_password(username) != password)
    {
        return NOK;
    }
    login_user(username);
    return OK;
};

/// 0 caso que login deu bem
/// 1 caso que o username nao esteja login
/// 2 user nao ten conta registada
/// 3 palavra-passe incorreta
int check_logout(string username, string password)
{
    if (!check_user_registered(username))
    {
        return UNR;
    }
    else if (!check_user_login(username))
    {
        return NOK;
    }
    else if (get_password(username) != password)
    {
        return WRP;
    }
    logout_user(username);
    return OK;
};

/// 0 caso que  deu bem
/// 1 caso que o username nao esteja login
/// 2 user nao ten conta registada
/// 3 palavra-passe incorreta
int check_unregister(string username, string password)
{
    if (!check_user_registered(username))
    {
        return UNR;
    }
    else if (!check_user_login(username))
    {
        return NOK;
    }
    else if (get_password(username) != password)
    {
        return WRP;
    }
    erase_user_dir(username);
    return OK;
};

/// 0 caso que  deu bem
/// 1 caso que o username nao esteja login
/// 2 palavras passes nao fazem match-up
/// 3 user nao existe
int check_changePass(string username, string password_old, string password_new)
{
    if (!check_user_registered(username))
    {
        return NID;
    }
    else if (!check_user_login(username))
    {
        return NLG;
    }
    else if (get_password(username) != password_old)
    {
        return NOK;
    }
    change_pass(username, password_new);
    return OK;
};

/// 0 caso que  deu bem
/// 1 caso que o username nao criou eventos
/// 2 user nao esta login
/// 3 palavra-passe incorreta
int check_myevents(string username, string password, string &msg)
{
    if (!check_user_registered(username) || !check_user_login(username))
    {
        return NLG;
    }
    else if (get_password(username) != password)
    {
        return WRP;
    }
    else if ((msg = get_user_events(username)).size() == 0)
    {
        return NOK;
    }
    return OK;
};

int check_close_event(string username, string password, string eid)
{
    /// 0 caso que  deu bem
    /// 1 caso que o username nao esta login
    /// 2 user nao existe ou palavra passe incorreta
    /// 3 evento nao existe
    /// 4 evento nao fui criado pelo user
    /// 5 SLD sold out
    /// 6 PST o evento ja passou
    /// 7 clo o evento ja tinha sido fechado
    if (!check_user_registered(username) || get_password(username) != password)
    {
        return 2;
    }
    else if (!check_user_login(username))
    {
        return 1;
    }
    else if (!check_event_existence(eid))
    {
        return 3;
    }
    else if (!check_is_user_event(username, eid))
    {
        return 4;
    }
    else if (check_event_closed(eid))
    {
        return 7;
    }
    else if (!check_event_future(eid))
    {
        return 6;
    }
    else if (!check_event_available_seats(eid))
    {
        return 5;
    }
    close_active_event(eid);
    return 0;
}

/// 0 caso que  deu bem
/// nao criou eventos
int check_list(string &msg)
{
    if ((msg = get_event_list()).size() == 0)
    {
        return NOK;
    }
    return OK;
};

/// 0 caso que  deu bem
/// 1 caso que o eid nao existe
int check_show(string eid, string &msg)
{
    if (!check_event_existence(eid))
    {
        return NOK;
    }
    msg = get_event_info(eid);
    return OK;
};

int check_myreservations(string username, string password, string &msg)
{
    /// 0 caso que  deu bem
    /// 1 caso que o username nao fez reservation
    /// 2 user nao esta login
    /// 3 palavra-passe incorreta
    if (!check_user_registered(username) || !check_user_login(username))
    {
        return 2;
    }
    else if (get_password(username) != password)
    {
        return 3;
    }
    else if ((msg = get_user_reservations(username)).size() == 0)
    {
        return 1;
    }
    return 0;
};

/// 0 caso que  deu bem ACC
/// 1 evento nao ativo NOK
/// 2 user not logged in NGL
/// 3 evento sold out SLD
/// 4 REJ value > lugares disponiveis
/// 5 palavra-passe incorreta
/// 6 PST evento já aconteceu
/// 7 clo o evento ja tinha sido fechado
int check_reserve(string username, string password, string eid, int value, int &msg)
{
    if (!check_user_registered(username) || !check_user_login(username))
    {
        return 2;
    }
    else if (get_password(username) != password)
    {
        return 5;
    }
    else if (!check_event_existence(eid))
    {
        return 1;
    }
    else if (check_event_closed(eid))
    {
        return 7;
    }
    else if (!check_event_future(eid))
    {
        return 6;
    }
    else if (!check_event_available_seats(eid))
    {
        return 3;
    }
    else if ((msg = get_available_seats(eid)) < value)
    {
        return 4;
    }
    reserve_event(username, eid, to_string(value));
    return 0;
}

/// 0 caso que  deu bem
/// 1 caso nao foi sucedido
/// 2 user nao esta login
/// 3 palavra-passe incorreta
int check_create_event(string username, string password, string event_name, string event_date,
                       string attendance_size, string file_name,
                       string file_size, string file_content, string &eid)
{
    if (!check_user_registered(username) || !check_user_login(username))
    {
        return 2;
    }
    else if (get_password(username) != password)
    {
        return 3;
    }
    else if ((eid = create_event_dir(username, event_name, event_date,
                                     attendance_size, file_name,
                                     file_size, file_content))
                 .size() == 0)
        return 1;
    return 0;
};