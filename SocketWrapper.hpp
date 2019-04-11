#ifndef SOCKET_WRAPPER_HPP
#define SOCKET_WRAPPER_HPP

#include <string>
#include <string.h> 
#include <sys/types.h> 
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>

using namespace std;

using SocketDescriptor = int;

class SocketWrapper {

    public:    
        SocketWrapper(int port);
        void setNumberOfClients(int numOfClients);
        SocketDescriptor getSocketDescriptor();
        SocketDescriptor acceptConnection(); 
        string readFromConnection(SocketDescriptor connection);
        void writeToConnection(SocketDescriptor connection, string message);
        void closeConnection(SocketDescriptor connection);

    private:
        const int MESSAGE_SIZE = 256;
        SocketDescriptor socketDescriptor;

        sockaddr_in buildAddress(int port);


};

#endif