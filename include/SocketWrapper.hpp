#ifndef SOCKET_WRAPPER_HPP
#define SOCKET_WRAPPER_HPP

#include <string>
#include <string.h> 
#include <sys/types.h> 
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include "Connection.hpp"
#include "Packet.hpp"
#include <fstream>


using namespace std;

using SocketDescriptor = int;

using File = FILE;
using Port = int;
using Hostname = char*;

class SocketWrapper {

    public:    
        const static int DEFAULT_PORT = 4000;
        
        void closeConnection(Connection connection);
    
        SocketDescriptor getSocketDescriptor();
        void closeSocket();

    protected:
        SocketDescriptor socketDescriptor;

        sockaddr_in buildDefaultAddress(int port);
        Packet* receivePacket(SocketDescriptor connectionDescriptor);
        bool sendPacket(SocketDescriptor connectionDescriptor, Packet* packet);
        bool sendFile(int command, SocketDescriptor connectionDescriptor, char* filename);

};

#endif