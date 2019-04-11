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

using namespace std;

using SocketDescriptor = int;

class SocketWrapper {

    public:    
        const static int DEFAULT_PORT = 4000;
        const static int MESSAGE_SIZE = 256;
        
        void closeConnection(Connection connection);
    
        SocketDescriptor getSocketDescriptor();
        void closeSocket();

    protected:
        SocketDescriptor socketDescriptor;

        sockaddr_in buildDefaultAddress(int port);
        string readFromConnection(SocketDescriptor connectionDescriptor);
        void writeToConnection(SocketDescriptor connectionDescriptor, string message);


};

#endif