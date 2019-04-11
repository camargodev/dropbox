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

        SocketWrapper();
        void setNumberOfClients(int numOfClients);
        Connection acceptConnection(); 
        string readFromServer();
        string readFromClient(Connection connection);
        void writeToServer(string message);
        void writeToClient(Connection connection, string message);
        void closeConnection(Connection connection);
        void configureAsServer(int port);
        void configureAsClient(string hostname, int port);
        SocketDescriptor getSocketDescriptor();
        void closeSocket();

    private:
        SocketDescriptor socketDescriptor;

        sockaddr_in buildAddress(in_addr hostname, int port);
        sockaddr_in buildAddress(int port);
        sockaddr_in buildDefaultAddress(int port);


};

#endif