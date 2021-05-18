#include "../include/tcp_server.h"

using namespace std;

char TCPServer::msg[MAXPACKETSIZE];
int TCPServer::num_client;
vector<descriptionSocket*> TCPServer::message;
vector<descriptionSocket*> TCPServer::newSockFd;
std::mutex TCPServer::mtx;

int TCPServer::setup(int port, const vector<int>& opts)
{
    int opt = 1;
    sockFd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockFd < 0) {
        std::cerr << "Error creating socket:" << strerror(errno) << std::endl;
        return -1;
    }
    memset(&serverAddress,0,sizeof(serverAddress));

    for (std::size_t i = 0; i < opts.size(); ++i) {
        if (setsockopt(sockFd, SOL_SOCKET, opts.size(), (char *)&opt, sizeof(opt)) < 0) {
            cerr << "error setsockopt" << endl;
            return -1;
        }
    }

    serverAddress.sin_family      = AF_INET;
    serverAddress.sin_addr.s_addr = htonl(INADDR_ANY);
    serverAddress.sin_port        = htons(port);
    if ((::bind(sockFd, (struct sockaddr *)&serverAddress, sizeof(serverAddress))) < 0) {
        cerr << "error bind" << endl;
        return -1;
    }

    if (listen(sockFd, 5) < 0) {
        cerr << "error listen" << endl;
        return -1;
    }
    num_client = 0;
    return 0;
}

void TCPServer::accepted()
{
    socklen_t addrSize = sizeof(clientAddress);
    auto* so           = new descriptionSocket;    // Why? so在task函数中被free了？图啥
    so->socket         = accept(sockFd, (struct sockaddr*)&clientAddress, &addrSize);
    so->id             = num_client;
    so->ip             = inet_ntoa(clientAddress.sin_addr);
    newSockFd.push_back(so);
    cerr << "accept client[ id:" << newSockFd[num_client]->id <<
         " ip:" << newSockFd[num_client]->ip <<
         " handle:" << newSockFd[num_client]->socket << " ]" << endl;
    pthread_create(&serverThread[num_client], nullptr, &Task, (void*)newSockFd[num_client]);
    num_client++;
}

/**
 * Each client has this thread function
 */
void* TCPServer::Task(void* arg)
{
    int n;
    auto *desc = (struct descriptionSocket*) arg;
    pthread_detach(pthread_self());

    cerr << "open client[ id:" << desc->id << " ip:" << desc->ip << " socket:" << desc->socket << " Send:" << desc->enableMessageRuntime << " ]" << endl;
    while (true) {
        n = recv(desc->socket, msg, MAXPACKETSIZE, 0);
        if (n != -1) {
            // If the connection with the client is broken
            if (n == 0) {
                cerr << "close client[ id:"<< desc->id <<" ip:"<< desc->ip <<" socket:"<< desc->socket<<" ]" << endl;
                close(desc->socket);
                int id = desc->id;
                auto new_end = std::remove_if(newSockFd.begin(), newSockFd.end(),
                                              [id](descriptionSocket* device){ return device->id == id; });
                newSockFd.erase(new_end, newSockFd.end());
                // Synchronous the thread array: pthread_t serverThread[MAX_CLIENT]
                if (num_client > 0) num_client--;
                break;
            }
            msg[n] = 0;
            desc->message = string(msg);
            std::lock_guard<std::mutex> guard(mtx);
            message.push_back(desc);
        }
        usleep(600);
    }
    free(desc);
    cerr << "exit thread: " << this_thread::get_id() << endl;
    pthread_exit(nullptr);
}

vector<descriptionSocket*> TCPServer::getMessage()
{
    std::lock_guard<std::mutex> guard(mtx);
    return message;
}

void TCPServer::Send(const string& tmpMsg, int id)
{
    send(newSockFd[id]->socket, tmpMsg.c_str(), tmpMsg.length(), 0);
}

void TCPServer::clean(int id)
{
    message[id]->message = "";
    memset(msg, 0, MAXPACKETSIZE);
}

string TCPServer::getIpAddr(int id)
{
    return newSockFd[id]->ip;
}

void TCPServer::detach(int id)
{
    close(newSockFd[id]->socket);
    newSockFd[id]->ip = "";
    newSockFd[id]->id = -1;
    newSockFd[id]->message = "";
}

void TCPServer::closed() const
{
    close(sockFd);
}
