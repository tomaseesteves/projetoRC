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

#include "parser.hpp"

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
    string response;
    
    fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd == -1)
    {
        cout << "Error creating socket.\n";
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
        cout << "Error establishing TCP connection.\n";
        exit(1);
    }

    // Write to TCP server
    while (total_bytes < msg_len)
    {
        n = write(fd, msg.c_str() + total_bytes, msg_len - total_bytes);
        if (n <= 0) 
        {
            cout << "Error sending message to TCP server.\n";
            exit(1);
        }
        total_bytes += n;
    }
    
    // Read messages from TCP server
    total_bytes = 0;
    while (true)
    {
        n = read(fd, buffer, MAX_STRING);
        if (n == -1)
        {
            cout << "Error receiving message from TCP server.\n";
            exit(1);
        }
        total_bytes += n;
        response.append(buffer, n);

        // Check if it's a RSE response
        if (response.find("RSE ", 0) != string::npos)
        {
            istringstream iss(response);
            string tag, status, userID, name, event_date, event_hour,
                attendance_size, seats_reserved, file_name, file_size;
 
            if (iss >> tag >> status >> userID >> name >> event_date >> event_hour
                >> attendance_size >> seats_reserved >> file_name >> file_size)
            {
                size_t already_read = response.size() - iss.tellg();

                while (already_read < (size_t)stoi(file_size))
                {
                    ssize_t m = read(fd, buffer, sizeof(buffer));
                    if (m <= 0)
                    {
                        perror("read");
                        exit(1);
                    }
                    response.append(buffer, m);
                    already_read += m;
                }
                break;
            }
        }

        // If not, check if newline received
        size_t pos = response.find('\n');
        if (pos != string::npos)
        {
            // Keep data up to '\n'
            response.resize(pos + 1);
            break;
        }
    }
    

    //cout << response; //so para testar

    freeaddrinfo(res);
    close(fd);

    return response;
}