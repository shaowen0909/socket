#include <iostream>
#include <csignal>
#include "include/tcp_client.h"

TCPClient tcp; /* NOLINT */

void sig_exit(int s)
{
    tcp.exit();
    exit(0);
}

int main(int argc, char *argv[])
{
    if (argc != 3){
        cerr << "Usage: ./client ip port" << endl;
        return 0;
    }
    signal(SIGINT, sig_exit);

    tcp.setup(argv[1],atoi(argv[2]));
    while (true)
    {
        std::string data;
        cin >> data;
        if (data == "quit")
            break;
        tcp.Send(data);
        /*string rec = tcp.receive();
        if (rec != ""){
            cout << rec << endl;
        }*/
        usleep(1000);
    }
    return 0;
}