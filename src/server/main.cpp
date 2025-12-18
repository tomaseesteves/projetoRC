#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <sstream>
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

#include <protocol.hpp>
#include <constants.hpp>

char port[MAX_STRING];
bool verbose = false;

// tcp_message_complete -> verifica se mensagem tcp terminou de ser escrita (CREATE para com tamanho de ficheiro, resto usa '\n')
// handle_request -> switch case

bool tcp_message_complete(string msg)
{
    // Check if it's not a create request
    if (msg.rfind("CRE ", 0) != 0) {
        return msg.find('\n') != string::npos;
    }

    // If it is, check if whole file has been received
    if (msg.find("CRE ", 0) != string::npos)
    {
        istringstream iss(msg);
        string tag, userID, password, name, event_date, event_hour,
               attendance_size, file_name;
        size_t file_size;

        // Check if header was fully sent
        if (!(iss >> tag >> userID >> password >> name >> event_date >> 
            event_hour >> attendance_size >> file_name >> file_size))
        {
            return false;
        }

        streampos header_end = iss.tellg();
        if (header_end == -1) {
            return false;
        }

        // Check if full file has been sent
        size_t bytes_available = msg.size() - (size_t)(header_end);
        return bytes_available >= file_size;
    }
}

int main(int argc, char **argv)
{
    
    bool exit_program = false;
    string input, command;
    vector<string> tokens;
    map<int, string> tcp_buffers;

    // No arguments, use default values for port
    if (argc == 1)
    { 
        strcpy(port, "58000");
    }
    // Use given port and activate verbose, if needed
    else if (argc == 2 && !strcmp(argv[1], "-v"))
    {
        verbose = true;
    }
    else if (argc == 3 && !strcmp(argv[1], "-p"))
    {
        strcpy(port, argv[2]);
    }
    else if (argc == 4 && !strcmp(argv[1], "-p") && !strcmp(argv[3], "-v"))
    {
        strcpy(port, argv[2]);
        verbose = true;
    }
    else 
    {
        cout << "Incorrect number of arguments.\nUsage: ./server [-p ESport] [-v]\n\n";
        exit(1);
    }

    fd_set inputs, testfds;
    struct timeval timeout;

    int i, out_fds, n, errcode, n_udp, n_tcp;

    char buffer[MAX_STRING];

    struct addrinfo udp_hints, *udp_res, tcp_hints, *tcp_res;
    struct sockaddr_in udp_useraddr, tcp_useraddr;
    socklen_t addrlen;
    int udp_fd, tcp_fd, newfd;

    char host[NI_MAXHOST], service[NI_MAXSERV];

    signal(SIGPIPE, SIG_IGN);

    // UDP Server
    memset(&udp_hints, 0, sizeof(udp_hints));
    udp_hints.ai_family = AF_INET;
    udp_hints.ai_socktype = SOCK_DGRAM;
    udp_hints.ai_flags = AI_PASSIVE|AI_NUMERICSERV;

    if ((errcode = getaddrinfo(NULL, port, &udp_hints, &udp_res)) != 0)
        exit(1);

    udp_fd = socket(udp_res->ai_family, udp_res->ai_socktype, udp_res->ai_protocol);
    if (udp_fd == -1)
        exit(1);

    if (bind(udp_fd, udp_res->ai_addr, udp_res->ai_addrlen) == -1)
    {
        cout << "Bind error UDP server\n";
        exit(1);
    }
    if (udp_res != NULL)
        freeaddrinfo(udp_res);

    // TCP Server
    memset(&tcp_hints, 0, sizeof(tcp_hints));
    tcp_hints.ai_family = AF_INET;
    tcp_hints.ai_socktype = SOCK_STREAM;
    tcp_hints.ai_flags = AI_PASSIVE|AI_NUMERICSERV;

    if ((errcode = getaddrinfo(NULL, port, &tcp_hints, &tcp_res)) != 0)
        exit(1);

    tcp_fd = socket(tcp_res->ai_family, tcp_res->ai_socktype, tcp_res->ai_protocol);
    if (tcp_fd == -1)
        exit(1);

    if (bind(tcp_fd, tcp_res->ai_addr, tcp_res->ai_addrlen) == -1)
    {
        cout << "Bind error TCP server\n";
        exit(1);
    }
    if (listen(tcp_fd, SOMAXCONN) == -1) 
    {
        exit(1);
    }

    if(tcp_res != NULL)
        freeaddrinfo(tcp_res);

    // Set input, UDP and TCP channel 
    FD_ZERO(&inputs); 
    FD_SET(0, &inputs); 
    FD_SET(udp_fd, &inputs); 
    FD_SET(tcp_fd, &inputs); 

    int maxfd = udp_fd;
    if (tcp_fd > maxfd) maxfd = tcp_fd;

    while(1)
    {
        testfds = inputs; 
        memset((void *)&timeout, 0, sizeof(timeout));
        timeout.tv_sec=10;

        out_fds = select(maxfd + 1, &testfds, (fd_set *)NULL, (fd_set *)NULL, (struct timeval *)&timeout);
        switch(out_fds)
        {
            case 0:
                break;
            case -1:
                exit(1);
            default:
                // Keyboard
                if(FD_ISSET(0, &testfds))
                {
                    if (!getline(cin, input)) 
                    {
                        FD_CLR(0, &inputs);
                    }
                    // string response = handle_request(input); -> envia para switch case, que depois envia para funcao especifica
                    cout << input + '\n';
                }
                // UDP socket
                if(FD_ISSET(udp_fd, &testfds))
                {
                    addrlen = sizeof(udp_useraddr);
                    memset(buffer, 0, sizeof(buffer));
                    n_udp = recvfrom(udp_fd, buffer, MAX_STRING, 0, (struct sockaddr *)&udp_useraddr, &addrlen);
                    if (n_udp == -1)
                    {
                        cout << "Error receiving UDP client message.";
                    }
                    string udp_client_msg(buffer, n_udp);
                    string response;
                    // string response = handle_request(udp_client_msg); -> envia para switch case, que depois envia para funcao especifica
                    
                    n_udp = sendto(udp_fd, response.c_str(), response.length(), 0, (struct sockaddr *)&udp_useraddr, addrlen);
                    if (n_udp == -1)
                    {
                        cout << "Error sending message to UDP client.";
                    }
                }
                // TCP listening socket
                if (FD_ISSET(tcp_fd, &testfds))
                {   
                    newfd = accept(tcp_fd, NULL, NULL);
                    if (newfd != -1) 
                    {
                        if (newfd < FD_SETSIZE) 
                        {
                            FD_SET(newfd, &inputs);
                            if (newfd > maxfd) maxfd = newfd;
                        } else {
                            close(newfd);
                        }
                    }
                }
                // TCP client sockets
                for (i = 0; i <= maxfd; i++) 
                {
                    if (i != tcp_fd && i != udp_fd && i != 0 && FD_ISSET(i, &testfds)) 
                    {
                        memset(buffer, 0, sizeof(buffer));
                        n_tcp = read(i, buffer, sizeof(buffer));
                        if (n_tcp <= 0) 
                        {
                            close(i);
                            FD_CLR(i, &inputs);
                            tcp_buffers.erase(i);
                            continue;
                        }

                        tcp_buffers[i].append(buffer, n_tcp);

                        if (tcp_message_complete(tcp_buffers[i])) 
                        {
                            string response;
                            //string response = handle_request(tcp_buffers[i]);
                            size_t sent_bytes = 0;
                            size_t response_len = response.length();

                            while (sent_bytes < response_len)
                            {
                                n_tcp = write(i, response.c_str() + sent_bytes, response_len - sent_bytes);
                                if (n_tcp <= 0) 
                                {
                                    FD_CLR(i, &inputs);
                                    tcp_buffers.erase(i);
                                    break;
                                }
                                sent_bytes += n_tcp;
                            }
                            tcp_buffers[i].clear();
                        }
                    }
                    if (i == maxfd) 
                    {
                        while (maxfd > 0 && !FD_ISSET(maxfd, &inputs))
                            maxfd--;
                    }
                }
        }
    }
}

