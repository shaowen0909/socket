#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>    // inet_ntoa()

using namespace std;
const int MAX_BYTE = 1024;
const int DEFAULT_PORT = 6666;
char buffer[MAX_BYTE];

int main()
{
    int listenFd, clientFd;
    struct sockaddr_in listenAddr{};
    struct sockaddr_in clientAddr{};
    int ret;

    //Create
    listenFd = socket(AF_LOCAL, SOCK_STREAM, 0);
    if (listenFd == -1){
        fprintf(stderr,"Error creating socket:%s\n", strerror(errno));
        exit(0);
    }
    listenAddr.sin_family = AF_LOCAL;    // AF_LOCAL is used for native communication and AF_INET is used for network communication
    listenAddr.sin_addr.s_addr = htonl(INADDR_ANY);    // Let the system automatically obtain the IP address of the machine
    listenAddr.sin_port = htons(DEFAULT_PORT);

    //Bind
    ret = bind(listenFd, reinterpret_cast<const sockaddr*>(&listenAddr), sizeof(listenAddr));
    if (ret == -1){
        fprintf(stderr, "Error binding:%s\n", strerror(errno));
        exit(0);
    }

    //Listen
    ret = listen(listenFd, 2);
    if (ret == -1){
        fprintf(stderr,"Error listening:%s\n", strerror(errno));
        exit(0);
    }

    //Accept
    cout<<"Waiting for a client connection request..."<<endl;
    while (true){
        memset(buffer, 0, sizeof(buffer));
        std::size_t lengthAddr = sizeof(clientAddr);
        clientFd = accept(listenFd, reinterpret_cast<sockaddr *>(&clientAddr),
                          reinterpret_cast<socklen_t *>(&lengthAddr));
        if (clientFd == -1){
            fprintf(stderr,"Error accepting%s\n", strerror(errno));
            exit(0);
        }
        fprintf(stdout,"Server get connection from %s\n", inet_ntoa(clientAddr.sin_addr));

        ret = recv(clientFd, buffer, MAX_BYTE, 0);
        if (ret == -1){
            fprintf(stderr,"Error receiving:%s\n", strerror(errno));
            exit(0);
        }
        buffer[ret]='\0';
        printf("I have received:%s\n", buffer);

        ret = send(clientFd, buffer, strlen(buffer), 0);
        if(ret == -1){
            fprintf(stderr,"Error writing:%s\n", strerror(errno));
            exit(0);
        }

        // At this point this communication has ended, Continue the loop and accept the next one
        close(clientFd);
    }
    // return 0;
}

/* Something want to say:
 *
 * If I have some codes looks like this:
 * if (a = socket(AF_LOCAL, SOCK_STREAM, 0) == -1){
 * ...
 * }
 * So the result is going to be that 'a' is either equal to 0 or equal to 1
 * The assignment symbol '=' has the lowest priority,
 * this leads to listenFd and clientFd in creating/connection success is 0, no success of 1.
 *
 * So the correct way to write is:
 * if ((a = socket(AF_LOCAL, SOCK_STREAM, 0)) == -1)
 * ...
 * */