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

#include <network.hpp>
#include <constants.hpp>

using namespace std;

int fd, errcode;
ssize_t msg;
socklen_t addrlen;
struct addrinfo hints, *res;
struct sockaddr_in addr;

string connect_UDP(char *ip_address, char *port, string msg)
{
    int msg_len = msg.length();
    char buffer[MAX_STRING];

    fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (fd == -1)
    {
        cout << "Error creating socket.\n";
        exit(1);
    }
    
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_DGRAM;
    
    errcode = getaddrinfo(ip_address, port, &hints, &res);
    if (errcode != 0)
    {
        cout << "Error establishing UDP connection.\n";
        exit(1);
    }
    
    ssize_t n = sendto(fd, msg.c_str(), msg_len, 0, res->ai_addr, res->ai_addrlen);
    if (n == -1)
    {
        cout << "Error sending message to UDP server.\n";
        exit(-1);
    }
    
    addrlen = sizeof(addr);
    n = recvfrom(fd, buffer, MAX_STRING, 0,
                 (struct sockaddr *)&addr, &addrlen);
    if (n == -1)
    {
        cout << "Error receiving message from UDP server.\n";
        exit(-1);
    }

    n = write(1, buffer, n); // só para testar
    if (n == -1)
    {
        cout << "Error writing message from UDP server.\n";
        exit(-1);
    }
    string response(buffer, n);

    freeaddrinfo(res);
    close(fd);

    return response;
}

string connect_TCP(char *ip_address, char *port, string msg)
{
    int msg_len = msg.length();
    char buffer[MAX_STRING];

    fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd == -1)
    {
        cout << "Erro a criar socket.\n";
        exit(1);
    }

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    errcode = getaddrinfo(ip_address, port, &hints, &res);
    if (errcode != 0)
    {
        cout << "Erro a estabelecer conexão TCP.\n";
        exit(1);
    }

    ssize_t n = connect(fd, res->ai_addr, res->ai_addrlen);
    ssize_t total_bytes = 0;

    if (n == -1)
    {
        cout << "Erro a tentar conectar com servidor TCP.\n";
        exit(1);
    }
    while (total_bytes != msg_len)
    {
        n = write(fd, msg.c_str(), msg_len);
        total_bytes += n;
    }

    if (n == -1)
    {
        cout << "Erro a enviar mensagem para servidor TCP.\n";
        exit(1);
    }
    // write e read Têm de ter loop para enviar todos os n dados
    while (total_bytes != MAX_STRING)
    {
        n = read(fd, buffer, MAX_STRING);
        total_bytes += n;
    }
    if (n == -1)
    {
        cout << "Erro a receber mensagem do servidor TCP.\n";
        exit(1);
    }

    n = write(1, buffer, n); // só para testar
    if (n == -1)
    {
        cout << "Erro a escrever mensagem para servidor UDP.\n";
        exit(-1);
    }
    string response = buffer;

    freeaddrinfo(res);
    close(fd);

    return response;
}