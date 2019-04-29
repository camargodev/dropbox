#ifndef SERVER_SOCKET_HPP
#define SERVER_SOCKET_HPP

#include "SocketWrapper.hpp"

using namespace std;

class ServerSocketWrapper : public SocketWrapper {

    public: 
        void listenOnPort(int port);
        bool openSocket();
        Packet* receivePacketFromClient(SocketDescriptor clientConnectionDescriptor);
        bool sendPacketToClient(SocketDescriptor clientConnectionDescriptor, Packet* packet);
        void setNumberOfClients(int numOfClients);
        Connection acceptClientConnection(); 
        bool sendFileToClient(SocketDescriptor clientConnectionDescriptor, char* filename);

    private:
        sockaddr_in buildAddress(int port);
        int port;

};

#endif