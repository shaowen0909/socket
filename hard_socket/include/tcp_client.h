#pragma once

#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <unistd.h>
#include <cstring>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <netdb.h>
#include <vector>

using namespace std;

class TCPClient
{
private:
    int sock;
    std::string address;
    int port;
    struct sockaddr_in server{};

public:
    TCPClient();
    bool setup(const string& addr, int Port);
    bool Send(const string& data) const;
    string receive(int size = 4096) const;
    string read() const;
    void exit() const;
};


