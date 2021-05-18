#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <cerrno>
#include <cstring>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

const int MAX_BYTE = 1024;
const int DEFAULT_PORT = 6666;
char buffer[MAX_BYTE];

using namespace std;

int main()
{
    int clientFd;
    struct sockaddr_in serverAddr{};

    // Create socket
    clientFd = socket(AF_LOCAL, SOCK_STREAM, 0);
    if(clientFd == -1){
        fprintf(stderr,"Error creating socket:%s\n",strerror(errno));
        exit(0);
    }

    // Initializes the server address to which you want to connect
    serverAddr.sin_family = AF_LOCAL;
    serverAddr.sin_port = htons(DEFAULT_PORT);
    serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);

    // Sending a connection request
    if((connect(clientFd, reinterpret_cast<const sockaddr *>(&serverAddr), sizeof(struct sockaddr))) == -1){
        fprintf(stderr,"Connect Error:%s\n", strerror(errno));
        exit(0);
    }

    // The connection is successful
    cout<<"Write something you want to say to her:"<<endl;
    std::cin>>buffer;
    cout<<"Sending..."<<endl;
    if((send(clientFd, buffer, strlen(buffer), 0)) == -1){
        fprintf(stderr,"Error writing:%s\n", strerror(errno));
        exit(0);
    }

    // Receive data in blocking mode
    int nBytes;
    if((nBytes = recv(clientFd, buffer, MAX_BYTE, 0)) == -1){
        fprintf(stderr,"Read Error:%s\n",strerror(errno));
        exit(1);
    }
    buffer[nBytes]='\0';
    printf("I have received:%s\n", buffer);

    // Close the socket
    close(clientFd);
    return 0;
}


