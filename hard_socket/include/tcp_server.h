#pragma once

#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <iostream>
#include <vector>
#include <thread>
#include <algorithm>
#include <cctype>
#include <mutex>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#define MAXPACKETSIZE 4096
#define MAX_CLIENT 100

struct descriptionSocket{
    int socket = -1;
    int id = -1;
    std::string ip;
    std::string message;
    bool enableMessageRuntime = false;
};

class TCPServer
{
public:
    static std::vector<descriptionSocket*> getMessage();
    static void clean(int id);
    static void Send(const std::string& msg, int id);
    static void detach(int id);
    static std::string getIpAddr(int id);

    int setup(int port, const std::vector<int>& opts = std::vector<int>());
    void accepted();
    void closed() const;

private:
    int sockFd;
    struct sockaddr_in serverAddress;
    struct sockaddr_in clientAddress;
    pthread_t serverThread[ MAX_CLIENT ];

    static char msg[ MAXPACKETSIZE ];
    static std::vector<descriptionSocket*> newSockFd;
    static std::vector<descriptionSocket*> message;    // The 'Task' function is responsible for updating it
    static int num_client;
    static std::mutex mtx;
    static void* Task(void* argv);
};