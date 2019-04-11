#ifndef CONNECTION_HPP
#define CONNECTION_HPP


using namespace std;

using SocketDescriptor = int;

struct Connection {

    SocketDescriptor descriptor;
    struct sockaddr_in address;
    socklen_t lenght;

};

#endif