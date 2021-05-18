#include <iostream>
#include <csignal>
#include "include/tcp_server.h"

using namespace std;

TCPServer tcp;

void close_app(int s) {
    tcp.closed();
    exit(0);
}

[[noreturn]] void* received(void* m) {
    pthread_detach(pthread_self());
    vector<descriptionSocket*> desc;
    while (true) {
        desc = TCPServer::getMessage();
        for (int i = 0; i < desc.size(); ++i) {
            if ( !desc[i]->message.empty() ) {
                cout << "id:      " << desc[i]->id      << endl
                     << "ip:      " << desc[i]->ip      << endl
                     << "message: " << desc[i]->message << endl
                     << "socket:  " << desc[i]->socket  << endl
                     << "enable:  " << desc[i]->enableMessageRuntime << endl;
                TCPServer::clean(i);
            }
        }
        usleep(1000);
    }
}

int main(int argc, char **argv)
{
    if ( argc != 2 ) {
        cerr << "Usage: ./server port" << endl;
        return 0;
    }
    std::signal(SIGINT, close_app);

    pthread_t msg;
    vector<int> opts = { SO_REUSEPORT, SO_REUSEADDR };
    // Call the function 'setup' to set the socket.
    if (tcp.setup(atoi(argv[1]), opts) == 0) {
        // Create a separate thread to print the information that has been received from the client.
        if (pthread_create(&msg, nullptr, received, (void *)nullptr) == 0){
            // Call "Accepted" to process the connection request from the client.
            while (true) {
                tcp.accepted();
            }
        }
    }
    else
        cerr << "Failed to setup!" << endl;
    return 0;
}